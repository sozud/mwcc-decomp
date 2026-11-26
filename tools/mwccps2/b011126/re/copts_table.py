#!/usr/bin/env python3
"""Dump b69's and 3.0-011126's compileroptions tables ({char* name; short bits}) side by side.

copts_table.py
"""

import difflib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "mwccpsp"))
from pe import PE  # noqa: E402


def dump(exe, va):
    pe = PE(exe)
    out = []
    while True:
        nm, bits = struct.unpack("<IH", pe.read(va, 6))
        if not nm:
            break
        out.append(
            f"{pe.read(nm, 64).split(bytes(1))[0].decode():28s} flags {bits & 0xF000:#06x} off {bits & 0xFFF:#05x}"
        )
        va += 6  # {char* name; short bits}, packed
    return out


a = dump(os.path.join(REPO, "orig", "PS2_3_0_3_b69", "mwccps2.exe"), int(sys.argv[1], 16))
b = dump(os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe"), int(sys.argv[2], 16))
print(len(a), len(b))
for l in difflib.unified_diff(a, b, "b69", "011126", n=0, lineterm=""):
    print(l)
