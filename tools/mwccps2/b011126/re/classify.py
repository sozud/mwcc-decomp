#!/usr/bin/env python3
"""Classify failing corpus cases: does each differing function differ on its own too?

    classify.py RUN.txt [-j N] [-o OUT.tsv]

For every obj-diff case in RUN.txt (corpus.sh's output), the functions whose code differs
between expected.o and this checkout's native output are found; each is then compiled
alone (every other function's body removed from the input, the rest of the file kept) by
the original 011126 under wibo and by the native. Per case:

  context   every differing function matches when compiled alone: the original's output
            depends on what it compiled before (its leftover memory, e.g. b69's arena
            hascall residue), not a code difference we can port
  code      some differing function still differs alone: a real difference
  unknown   the reduced file doesn't compile, or no differing function was found
"""

import argparse
import concurrent.futures as cf
import json
import os
import re
import shutil
import subprocess
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
CORPUS = os.environ.get("CORPUS", os.path.expanduser("~/Downloads/mweppc-b011126/build/mwccps2-b011126-corpus"))
OD = os.environ.get("OD", os.path.expanduser("~/Downloads/mipsel-binutils/bin/mipsel-linux-gnu-objdump"))
WIBO = os.path.join(REPO, "build", "tools", "wibo")
ORIG = os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe")
NATIVE = os.path.join(REPO, "build", "PS2_3_0_011126", "native", "x86_64", "mwccps2")


def funcs(obj):
    """{function: [instruction lines]} with addresses and encodings stripped"""
    p = subprocess.run([OD, "-d", "-r", "-z", "-m", "mips:5900", obj], capture_output=True, text=True)
    out, cur = {}, None
    for line in p.stdout.splitlines():
        m = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
        if m:
            cur = m.group(1)
            out[cur] = []
            continue
        if cur and line.strip():
            line = re.sub(r"^\s*[0-9a-f]+:\t([0-9a-f]{8} \t?)?", "", line)
            line = re.sub(r"<[^>]*\+0x[0-9a-f]+>", "", re.sub(r"^\s*[0-9a-f]+: ", "", line))
            out[cur].append(line)
    return out


def spans(src):
    """(start, end, name) of every top-level function definition"""
    out, i = [], 0
    while i < len(src):
        if re.match(r"^(static |inline |extern )*[A-Za-z_][\w \*]*\([^;]*\)\s*\{?\s*$", src[i]) and not src[
            i
        ].startswith(("typedef", "struct", "#")):
            j, depth, seen = i, 0, False
            while j < len(src):
                depth += src[j].count("{") - src[j].count("}")
                seen |= "{" in src[j]
                if seen and depth == 0:
                    break
                j += 1
            if seen and j < len(src):
                m = re.search(r"(\w+)\s*\(", src[i])
                out.append((i, j, m.group(1)))
                i = j + 1
                continue
        i += 1
    return out


def compile_both(work, argv, src_rel):
    res = []
    env = dict(os.environ, MWCIncludes=".")
    for cc in ([WIBO, ORIG], [NATIVE]):
        o = os.path.join(work, ".out", "r%d.o" % len(res))
        a = [x.replace("{input}", src_rel).replace("{output}", o) for x in argv[1:]]
        p = subprocess.run(["nice", "-n", "19", *cc, *a], cwd=work, capture_output=True, env=env, timeout=300)
        if p.returncode or not os.path.exists(o):
            return None
        res.append(funcs(o))
    return res


def classify(cid):
    case = os.path.join(CORPUS, cid)
    meta = json.load(open(os.path.join(case, "args.json")))
    work = tempfile.mkdtemp(prefix="cls-")
    try:
        if os.path.isdir(os.path.join(case, "root")):
            shutil.copytree(os.path.join(case, "root"), work, dirs_exist_ok=True)
        a = meta["argv"]
        for i, x in enumerate(a):
            d = x[2:] if x.startswith("-I") else (a[i + 1] if x == "-i" and i + 1 < len(a) else None)
            if d and not os.path.isabs(d) and not d.startswith("{"):
                os.makedirs(os.path.join(work, d), exist_ok=True)
        os.makedirs(os.path.join(work, ".in"))
        os.makedirs(os.path.join(work, ".out"))
        name = meta.get("input_name", "input.c")
        src = open(os.path.join(case, "input.c"), encoding="latin1").read().split("\n")
        rel = os.path.join(".in", name)
        open(os.path.join(work, rel), "w", encoding="latin1").write("\n".join(src))
        full = compile_both(work, meta["argv"], rel)
        if not full:
            return cid, "unknown", "full file doesn't compile"
        o, n = full
        bad = sorted(f for f in o if o[f] != n.get(f))
        if not bad:
            return cid, "unknown", "no differing function"
        sp = spans(src)
        verdicts = []
        for f in bad:
            keep = [s for s in sp if s[2] == f]
            if not keep:
                verdicts.append((f, "unknown"))
                continue
            out, cur = [], 0
            for i, j, nm in sp:
                out += src[cur:i]
                if nm == f:
                    out += src[i : j + 1]
                cur = j + 1
            out += src[cur:]
            open(os.path.join(work, rel), "w", encoding="latin1").write("\n".join(out))
            r = compile_both(work, meta["argv"], rel)
            if not r or f not in r[0]:
                verdicts.append((f, "unknown"))
            else:
                verdicts.append((f, "context" if r[0][f] == r[1].get(f) else "code"))
        kinds = {v for _, v in verdicts}
        k = "code" if "code" in kinds else ("unknown" if "unknown" in kinds else "context")
        return cid, k, " ".join(f"{f}:{v}" for f, v in verdicts)
    except Exception as e:  # noqa: BLE001
        return cid, "unknown", f"error {e}"
    finally:
        shutil.rmtree(work, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("run")
    ap.add_argument("-j", type=int, default=3)
    ap.add_argument("-o", default=os.path.join(REPO, "build", "classify.tsv"))
    a = ap.parse_args()
    cases = []
    for line in open(a.run):
        f = line.split()
        if f and f[0] == "obj-diff":
            cases.append((next(c for c in os.listdir(CORPUS) if c.startswith(f[1])), f[2]))
    counts = {}
    with open(a.o, "w") as out, cf.ThreadPoolExecutor(a.j) as ex:
        for (cid, kind, info), (_, target) in zip(ex.map(classify, [c for c, _ in cases]), cases):
            counts[kind] = counts.get(kind, 0) + 1
            out.write(f"{cid[:12]}\t{kind}\t{target}\t{info}\n")
            out.flush()
    print(counts, "->", a.o)


if __name__ == "__main__":
    main()
