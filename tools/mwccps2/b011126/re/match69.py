#!/usr/bin/env python3
"""Match b69's functions (tools/mwccps2/b69/functions.csv) to 3.0-011126's code.

    match69.py [-o OUT.csv]

Bytes are compared with relocated dwords and call/jmp rel32 operands masked. Walks b69's
functions in address order, trying each at the running delta first, then nearby.
Writes b69_start,b69_size,new_start,status (same / moved / none).
"""

import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "mwccpsp"))
from pe import PE  # noqa: E402


def load(path):
    pe = PE(path)
    data = open(path, "rb").read()
    text = next(s for s in pe.sections if s[0] == ".text")
    _, va, vsize, raw, _ = text
    base = pe.base
    code = bytearray(data[raw : raw + vsize])
    start = base + va
    mask = bytearray(len(code))
    for r in pe.relocations():
        o = r - start
        if 0 <= o < len(code):
            mask[o : o + 4] = b"\1\1\1\1"
    return start, bytes(code), mask


def norm(code, mask, off, size):
    b = bytearray(code[off : off + size])
    m = mask[off : off + size]
    for i in range(len(b)):
        if m[i]:
            b[i] = 0
    i = 0
    while i < len(b) - 4:
        if b[i] in (0xE8, 0xE9) and not m[i]:
            b[i + 1 : i + 5] = b"\0\0\0\0"
            i += 5
        else:
            i += 1
    return bytes(b)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-o", default=os.path.join(HERE, "..", "match69.csv"))
    ap.add_argument("--window", type=int, default=0x6000)
    a = ap.parse_args()
    s69, c69, m69 = load(os.path.join(REPO, "orig", "PS2_3_0_3_b69", "mwccps2.exe"))
    sN, cN, mN = load(os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe"))
    funcs = [
        (int(r["start"], 16), int(r["size"]))
        for r in csv.DictReader(open(os.path.join(REPO, "tools", "mwccps2", "b69", "functions.csv")))
    ]
    delta = sN - s69
    rows = []
    counts = {"same": 0, "moved": 0, "none": 0}
    for st, sz in funcs:
        want = norm(c69, m69, st - s69, sz)
        exp = st + delta - sN
        got = None
        if norm(cN, mN, exp, sz) == want:
            got = exp
        else:
            for d in range(4, a.window, 1):
                for o in (exp - d, exp + d):
                    if 0 <= o and norm(cN, mN, o, sz) == want:
                        got = o
                        break
                if got is not None:
                    break
        if got is None:
            rows.append((st, sz, None, "none"))
            counts["none"] += 1
            continue
        status = "same" if got == exp else "moved"
        counts[status] += 1
        delta = got + sN - st
        rows.append((st, sz, got + sN, status))
    with open(a.o, "w") as f:
        f.write("b69_start,b69_size,new_start,status\n")
        for st, sz, n, s in rows:
            f.write(f"{st:08x},{sz},{'' if n is None else f'{n:08x}'},{s}\n")
    print(counts, "->", a.o)


if __name__ == "__main__":
    main()
