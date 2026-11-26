#!/usr/bin/env python3
"""For each b69 function 3.0-011126 changed, is 011126's code 2.3.3's?

    match233.py

The function's name (b69's symbols) gives its 2.3.3 address (b233's symbols) and size
(b233/functions.csv); 011126's code at the start of its gap (fdiff.ranges) is compared with
2.3.3's, relocated dwords and call/jmp operands masked (match69.norm). Prints per function:
  same233   011126's bytes are 2.3.3's (take 2.3.3's code: extend its MWCC_CORE_B233 guards)
  size233   same size as 2.3.3's but different bytes
  -         differs from both (or no 2.3.3 name)
"""

import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fdiff  # noqa: E402
import match69  # noqa: E402

B = fdiff.B
REPO = fdiff.REPO


def names(d):
    out = {}
    for f in ("symbols.manual.txt", "symbols.derived.txt", "symbols.txt", "symbols.guess.txt", "symbols.guess145.txt"):
        p = os.path.join(d, f)
        if os.path.exists(p):
            for line in open(p):
                m = re.match(r"([\w./+$?@-]+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line.split("//")[0].strip())
                if m:
                    out.setdefault(m.group(1).split("/")[-1], int(m.group(2), 16))
    return out


def main():
    s69 = fdiff.load_syms(os.path.join(os.path.dirname(B), "b69"))
    n233 = names(os.path.join(os.path.dirname(B), "b233"))
    sz233 = {
        int(r["start"], 16): int(r["size"])
        for r in csv.DictReader(open(os.path.join(os.path.dirname(B), "b233", "functions.csv")))
    }
    s3, c3, m3 = match69.load(os.path.join(REPO, "orig", "PS2_2_3_3", "mwccps2.exe"))
    sN, cN, mN = match69.load(os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe"))
    rg = fdiff.ranges()
    counts = {}
    for va, r in sorted(rg.items()):
        if len(r) != 5:
            continue
        n, lo, hi, p69, n69 = r
        nm = s69.get(va, "?")
        a3 = n233.get(nm)
        st = "-"
        if a3 is not None and a3 in sz233:
            k = sz233[a3]
            # the function's start in 011126: the gap start shifted by its distance in b69's gap
            for start in sorted({lo, lo + (va - p69)}):
                if match69.norm(cN, mN, start - sN, k) == match69.norm(c3, m3, a3 - s3, k):
                    st = "same233"
                    break
            else:
                if k == (hi - lo) - (n69 - p69) + n:
                    st = "size233"
        counts[st] = counts.get(st, 0) + 1
        print(f"{va:08X} {st:8s} {nm}" + (f" (2.3.3 {a3:08X})" if a3 else ""))
    print(counts, file=sys.stderr)


if __name__ == "__main__":
    main()
