#!/usr/bin/env python3
"""replay.py CASES NATIVE OUT: per case, record the original b119's residue
words (resid.py) and run the native with them replayed
(MWCCPS2_B119_RESIDUE). Prints per case: pass / fail, and the hit count.
RESID_CACHE=DIR keeps the recorded words (DIR/<case>.seq) and reuses them:
they depend only on the original and the case, not on the native."""

import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "../../../.."))
CORPUS = os.path.join(ROOT, "build/mwccps2-b119-corpus")
EXE = os.path.join(ROOT, "orig/PS2_3_0_1_b119/mwccps2.exe")
WIBO = os.environ.get("WIBO", "/opt/tools/wibo")
sys.path.insert(0, os.path.join(ROOT, "tools/mwccpsp/corpus"))


def setup(case, work):
    c = os.path.join(CORPUS, case)
    if os.path.isdir(os.path.join(c, "root")):
        shutil.copytree(os.path.join(c, "root"), work, dirs_exist_ok=True)
    m = json.load(open(os.path.join(c, "args.json")))
    name = m.get("input_name", "input.c")
    os.makedirs(os.path.join(work, ".in"), exist_ok=True)
    os.makedirs(os.path.join(work, ".out"), exist_ok=True)
    shutil.copy(os.path.join(c, "input.c"), os.path.join(work, ".in", name))
    args = [
        x.replace("{input}", os.path.join(work, ".in", name))
        .replace("{output}", os.path.join(work, ".out", "r.o"))
        .replace("{cwd}", work)
        for x in m["argv"][1:]
    ]
    for i, x in enumerate(args):
        d = x[2:] if x.startswith("-I") else (args[i + 1] if x == "-i" and i + 1 < len(args) else None)
        if d and not os.path.isabs(d):
            os.makedirs(os.path.join(work, d), exist_ok=True)
    return args


def main():
    cases, native, out = sys.argv[1], os.path.abspath(sys.argv[2]), sys.argv[3]
    res = open(out, "w")
    full = os.listdir(CORPUS)
    for pre in open(cases).read().split():
        case = next(d for d in full if d.startswith(pre))
        cache = os.environ.get("RESID_CACHE")
        seq = os.path.join(cache, f"{case}.seq") if cache else os.path.join(tempfile.gettempdir(), f"resid-{case}.seq")
        if not (cache and os.path.exists(seq)):
            work = tempfile.mkdtemp(prefix="resid-")
            args = setup(case, work)
            subprocess.run(
                [sys.executable, os.path.join(HERE, "resid.py"), seq, "--", WIBO, EXE, *args],
                cwd=work,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                timeout=1800,
            )
            shutil.rmtree(work)
        n = sum(1 for _ in open(seq)) if os.path.exists(seq) else -1
        r = subprocess.run(
            [
                sys.executable,
                os.path.join(ROOT, "tools/mwccpsp/corpus/run_corpus.py"),
                "--corpus",
                CORPUS,
                "--cc",
                native,
                "-j",
                "1",
                "--cases",
                "/dev/stdin",
            ]
            + (["--keep"] if os.environ.get("REPLAY_KEEP") else []),
            input=case,
            text=True,
            capture_output=True,
            env={**os.environ, "MWCCPS2_B119_RESIDUE": seq},
            timeout=1800,
        )
        ok = r.stdout.strip().splitlines()[-1].startswith("1/1") if r.stdout.strip() else False
        mis = open(seq + ".mis").read().strip() if os.path.exists(seq + ".mis") else ""
        os.unlink(seq + ".mis") if mis else None
        res.write(f"{case}\t{'pass' if ok else 'fail'}\t{n}\t{mis}\n")
        res.flush()
        if not cache and os.path.exists(seq):
            os.unlink(seq)


main()
