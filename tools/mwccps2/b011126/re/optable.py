#!/usr/bin/env python3
"""Decode the opcode table initializer of b69 and 3.0-011126 and diff them.

    optable.py [--dump b69|011126]

The initializer (b69 0x48EA60, 011126 0x48EB10) is a memset of the table, then `orl $v, table+i*0x2C+f`
stores and calls whose pushed arguments are immediates and string pointers. Each event is printed with
strings resolved and table addresses as [opcode].field, so the two builds compare entry by entry.
Conditional parts (option tests) show as their compare/jump instructions.
"""

import argparse
import difflib
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "mwccpsp"))
from pe import PE  # noqa: E402

BUILDS = {
    "b69": (os.path.join(REPO, "orig", "PS2_3_0_3_b69", "mwccps2.exe"), 0x48EA60, 0x497800),
    "011126": (os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe"), 0x48EB10, 0x4978A0),
}
ENTRY = 0x2C


def decode(build):
    exe, lo, hi = BUILDS[build]
    pe = PE(exe)
    p = subprocess.run(
        ["objdump", "-d", "--no-show-raw-insn", f"--start-address=0x{lo:x}", f"--stop-address=0x{hi:x}", exe],
        capture_output=True,
        text=True,
    )
    lines = [
        l.split("\t", 1)[1].split("#")[0].strip()
        for l in p.stdout.splitlines()
        if re.match(r"\s*[0-9a-f]+:\s", l) and "\t" in l
    ]
    base = None
    stack = []
    out = []
    callees = {}

    def s(v):
        try:
            b = pe.read(v, 80).split(b"\0")[0]
            if b and all(32 <= c < 127 for c in b):
                return repr(b.decode())
        except Exception:
            pass
        return hex(v)

    def tbl(v):
        if base is not None and base <= v < base + ENTRY * 0x600:
            return f"[{(v - base) // ENTRY:#x}]+{(v - base) % ENTRY:#x}"
        return hex(v)

    for ins in lines:
        m = re.match(r"pushl\s+\$(-?0x[0-9a-f]+|\d+)$", ins)
        if m:
            stack.append(int(m.group(1), 16) & 0xFFFFFFFF if "0x" in m.group(1) else int(m.group(1)))
            continue
        m = re.match(r"calll\s+0x([0-9a-f]+)", ins)
        if m:
            t = int(m.group(1), 16)
            if base is None and len(stack) >= 3:
                base = stack[-1]
                out.append(f"memset size {stack[-3]:#x}")
            else:
                callees.setdefault(t, f"f{len(callees)}")
                out.append(
                    f"call {callees[t]}(" + ", ".join(s(v) if v > 0x400000 else hex(v) for v in reversed(stack)) + ")"
                )
            stack = []
            continue
        if ins.startswith("addl") and "%esp" in ins:
            continue
        m = re.match(r"orl\s+\$(0x[0-9a-f]+|\d+), (0x[0-9a-f]+)$", ins)
        if m:
            out.append(f"or {tbl(int(m.group(2), 16))} |= {m.group(1)}")
            continue
        out.append("  " + re.sub(r"0x[0-9a-f]{6}\b", lambda mm: tbl(int(mm.group(0), 16)), ins))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dump")
    a = ap.parse_args()
    if a.dump:
        print("\n".join(decode(a.dump)))
        return
    x, y = decode("b69"), decode("011126")
    for line in difflib.unified_diff(x, y, "b69", "011126", n=1, lineterm=""):
        print(line)


if __name__ == "__main__":
    main()
