#!/usr/bin/env python3
"""Write a copy of 011126's mwccps2.exe whose constant ENodes get hascall = 0, as the native's do.

    hascall0.py OUTDIR

primary_expression lallocs an ENode for an int constant (0x469590) and a float constant
(0x4695D0) and writes its type, +1 and the flags word (+2) but not +4/+5 (ignored, hascall),
so hascall is whatever an earlier allocation left in lalloc's arena (b69's arena residue,
../../b69/corpus-ub.txt). call_function evaluates an argument whose hascall byte is set
before the others, so the residue reorders argument set-up. The patch rewrites the 10 bytes
`movb $0,1(%ebx); movw $0,2(%ebx)` at 0x46959D and 0x4695DD as
`xorl %eax,%eax; movl %eax,2(%ebx); movb %al,1(%ebx); nop; nop` (+1..+5 zero; eax is dead
there: the next call returns into it). The native takes 0 for hascall (CExpr.c), so a case
the patched original compiles to the native's object differs only by that residue.
"""

import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "mwccpsp"))
from pe import PE  # noqa: E402

OLD = bytes.fromhex("c643010066c743020000")
NEW = bytes.fromhex("31c08943028843019090")
SITES = (0x46959D, 0x4695DD)


def main():
    out = sys.argv[1]
    src = os.path.join(REPO, "orig", "PS2_3_0_011126")
    os.makedirs(out, exist_ok=True)
    for f in os.listdir(src):
        shutil.copyfile(os.path.join(src, f), os.path.join(out, f))
    exe = os.path.join(out, "mwccps2.exe")
    pe = PE(exe)
    data = bytearray(open(exe, "rb").read())
    for va in SITES:
        o = pe.off(va - pe.base)
        if data[o : o + len(OLD)] != OLD:
            sys.exit(f"0x{va:X}: unexpected bytes {data[o : o + len(OLD)].hex()}")
        data[o : o + len(NEW)] = NEW
    open(exe, "wb").write(data)
    print(exe)


if __name__ == "__main__":
    main()
