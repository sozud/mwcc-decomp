#!/usr/bin/env python3
"""Capture a golden corpus for mwccps2 3.0 011126 (Persona 3 FES's game code) from the corpora we have.

    capture.py [--out build/mwccps2-b011126-corpus] [-j 6] [LABEL=DIR ...]

(tools/mwccps2/b011126; adapted from the sotn-decomp tree's capture_b198.py.) The default source is
the b198 corpus, which already holds every PS2 corpus's inputs once (Persona 4, Persona 3 FES, Yakuza,
Monster Hunter, Silent Hill 3, Duelists of the Roses).

No new game code is captured: every case of the existing PS2 corpora (the
default sources below, or LABEL=DIR arguments) is replayed with its own input,
headers and argv through the original 011126 (orig/PS2_3_0_011126/mwccps2.exe) under
wibo, exactly as tools/mwccpsp/corpus/run_corpus.py runs a case (fresh work
directory, input at .in/<input_name>), so the expected objects are 011126's.

- Duplicates (same argv, input name, input and header tree) are written once;
  args.json records every source the case came from ("b198_sources").
- Cases 011126 rejects (unknown options or pragmas, errors) go to rejected.txt.
- Determinism: each case is compiled a second time from a work directory with a
  much longer path (the older builds' outputs depend on stack residue that
  moves with the path). A case whose two objects (or depfiles) differ is still
  written, and listed in nondeterministic.txt: the original's output depends on
  its own undefined behaviour there, so it is skipped (corpus-ub.txt).

Existing case directories are kept (rerunning only adds what is missing).
"""

import argparse
import concurrent.futures as cf
import hashlib
import json
import os
import shutil
import subprocess
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))

EXE = os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe")
WIBO = os.environ.get("WIBO", os.path.join(REPO, "build", "tools", "wibo"))
TIMEOUT = int(os.environ.get("CORPUS_TIMEOUT", "400"))
DL = os.path.expanduser("~/Downloads")
SOURCES = [
    ("b198", f"{DL}/sotn-mwccps2-b198/build/mwccps2-b198-corpus"),  # every PS2 corpus's inputs
]
# the second run's work directory: as long as a deep CI checkout's
LONG = "b011126-determinism-check-" + "x" * 96 + "-"


def dep_paths(dep_text, cwd):
    """Dependencies from an mwcc depfile, as paths relative to cwd (tools/mwccpsp/corpus/capture_wibo.py).
    Under wibo the names are Windows paths on drive Z: (Z:\\a\\b.h is /a/b.h); the first is the source."""
    _, _, body = dep_text.partition(": ")
    names = [n for n in body.replace("\r\n", "\n").replace("\\\n", " ").split() if n != "\\"][1:]
    out = []
    for n in names:
        if len(n) > 2 and n[1] == ":":
            n = n[2:].replace("\\", "/")
        p = os.path.normpath(n)
        if os.path.isabs(p):
            p = os.path.relpath(os.path.realpath(p), os.path.realpath(cwd))
        out.append(p)
    return out


def case_key(case_dir, meta):
    """What the compiler sees: argv, input name, input bytes and every header under root/."""
    h = hashlib.sha1()
    h.update(json.dumps([meta["argv"][1:], meta.get("input_name", "input.c")]).encode())
    h.update(open(os.path.join(case_dir, "input.c"), "rb").read())
    root = os.path.join(case_dir, "root")
    for r, ds, fs in os.walk(root):
        ds.sort()
        for f in sorted(fs):
            p = os.path.join(r, f)
            h.update(b"\0" + os.path.relpath(p, root).encode() + b"\0" + open(p, "rb").read())
    return h.hexdigest()


def compile_case(case_dir, meta, prefix):
    """Run one case through the original 011126; returns (status, obj bytes, dep bytes, work dir, info)."""
    work = tempfile.mkdtemp(prefix=prefix)
    root = os.path.join(case_dir, "root")
    if os.path.isdir(root):
        shutil.copytree(root, work, dirs_exist_ok=True)
    args = meta["argv"]
    for i, arg in enumerate(args):
        d = arg[2:] if arg.startswith("-I") else (args[i + 1] if arg == "-i" and i + 1 < len(args) else None)
        if d and not os.path.isabs(d) and not d.startswith("{"):
            os.makedirs(os.path.join(work, d), exist_ok=True)
    src = os.path.join(work, ".in", meta.get("input_name", "input.c"))
    os.makedirs(os.path.dirname(src))
    shutil.copyfile(os.path.join(case_dir, "input.c"), src)
    obj = os.path.join(work, ".out", "result.o")
    os.makedirs(os.path.dirname(obj))
    argv = [a.replace("{input}", src).replace("{output}", obj) for a in args[1:]]
    try:
        p = subprocess.run([WIBO, EXE, *argv], cwd=work, capture_output=True, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        return "timeout", None, None, work, ""
    if p.returncode or not os.path.isfile(obj):
        return "rejected", None, None, work, (p.stdout + p.stderr).decode("latin1").strip()[-300:]
    dep = None
    if meta.get("depfile_rel"):
        d = (
            meta["depfile_rel"]
            .replace("{cwd}", work)
            .replace("{output_stem}", os.path.splitext(obj)[0])
            .replace("{output}", obj)
        )
        if not os.path.isfile(d):
            return "rejected", None, None, work, "no depfile"
        dep = open(d, "rb").read()
    return "ok", open(obj, "rb").read(), dep, work, ""


def replay(label, case_dir, out, seen):
    meta = json.load(open(os.path.join(case_dir, "args.json")))
    key = case_key(case_dir, meta)
    tag = f"{label}:{os.path.basename(case_dir)}"
    target = meta.get("target") or ""
    if seen.setdefault(key, tag) != tag:
        return key, "dup", tag, target
    dst = os.path.join(out, key)
    if os.path.isdir(dst):
        return key, "have", tag, target
    st, obj, dep, work, info = compile_case(case_dir, meta, "b011126-case-")
    try:
        if st != "ok":
            return key, st, tag, f"{target}: {info}"
        st2, obj2, dep2, work2, _ = compile_case(case_dir, meta, LONG)
        shutil.rmtree(work2, ignore_errors=True)
        dep2n = dep2.replace(os.path.basename(work2).encode(), os.path.basename(work).encode()) if dep2 else dep2
        nondet = st2 != "ok" or obj2 != obj or dep2n != dep
        tmp = dst + f".tmp{os.getpid()}"
        shutil.copytree(
            case_dir, tmp, ignore=shutil.ignore_patterns("actual.o", "actual_deps.json", "expected.o", "expected.d")
        )
        open(os.path.join(tmp, "expected.o"), "wb").write(obj)
        if dep is not None:
            open(os.path.join(tmp, "expected.d"), "wb").write(dep)
            meta["headers"] = dep_paths(dep.decode("latin1"), work)
        meta["argv"][0] = "mwccps2.exe"
        src_tag = meta.get("b198_source")
        for k in ("b52", "b69", "b119", "b145", "b198", "b233", "b198_source"):
            meta.pop(k, None)
        meta["b011126"] = True
        meta["b011126_source"] = src_tag or tag
        json.dump(meta, open(os.path.join(tmp, "args.json"), "w"), indent=1)
        os.rename(tmp, dst)
        return key, "nondet" if nondet else "ok", tag, target
    finally:
        shutil.rmtree(work, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sources", nargs="*", help="LABEL=DIR (default: every existing PS2 corpus)")
    ap.add_argument("--out", default=os.path.join(REPO, "build", "mwccps2-b011126-corpus"))
    ap.add_argument("-j", type=int, default=6)
    ap.add_argument("--limit", type=int)
    a = ap.parse_args()
    sources = [tuple(s.split("=", 1)) for s in a.sources] or SOURCES
    os.makedirs(a.out, exist_ok=True)
    seen, lines, sources_of = {}, {"rejected": [], "nondet": []}, {}
    summary = []
    for label, d in sources:
        cases = sorted(os.path.join(d, c) for c in os.listdir(d) if os.path.isfile(os.path.join(d, c, "args.json")))[
            : a.limit
        ]
        # keys are claimed in order, so duplicates within the sources resolve the same way every run
        stats = {}
        with cf.ThreadPoolExecutor(a.j) as ex:
            for key, st, tag, info in ex.map(lambda c: replay(label, c, a.out, seen), cases):
                stats[st] = stats.get(st, 0) + 1
                sources_of.setdefault(key, []).append(tag)
                if st in ("rejected", "timeout"):
                    lines["rejected"].append(f"{tag} {st} {info.splitlines()[0] if info else ''}")
                elif st == "nondet":
                    lines["nondet"].append(f"{key} {tag} {info}")
        line = f"{label}: {len(cases)} cases: " + ", ".join(f"{k} {v}" for k, v in sorted(stats.items()))
        print(line, flush=True)
        summary.append(line)
    for name, key in (("rejected.txt", "rejected"), ("nondeterministic.txt", "nondet")):
        p = os.path.join(a.out, name)
        old = [l.rstrip("\n") for l in open(p)] if os.path.exists(p) else []
        keep = {l.split()[0]: l for l in old}
        keep.update({l.split()[0]: l for l in lines[key]})
        open(p, "w").write("".join(keep[k] + "\n" for k in sorted(keep)))
    json.dump(sources_of, open(os.path.join(a.out, "sources.json"), "w"), indent=0, sort_keys=True)
    open(os.path.join(a.out, "summary.txt"), "a").write("".join(l + "\n" for l in summary))


if __name__ == "__main__":
    main()
