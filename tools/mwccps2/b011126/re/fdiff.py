#!/usr/bin/env python3
"""Diff a b69 function's x86 code against its 3.0-011126 counterpart.

    fdiff.py B69_ADDR [--context N]      one function
    fdiff.py --list                       every unmatched b69 function with its 011126 range and size change

The 011126 range of an unmatched function is the gap between its matched neighbours (match69.csv).
Addresses in operands are replaced by names where symbols know them (b69 names for b69, the
translated names for 011126), so the diff shows what changed rather than where things moved.
"""

import argparse
import csv
import difflib
import os
import re
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
B = os.path.normpath(os.path.join(HERE, ".."))
REPO = os.path.normpath(os.path.join(B, "..", "..", ".."))
EXE69 = os.path.join(REPO, "orig", "PS2_3_0_3_b69", "mwccps2.exe")
EXEN = os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe")


def load_syms(d):
    out = {}
    for f in ("symbols.manual.txt", "symbols.derived.txt", "symbols.txt", "symbols.guess.txt", "symbols.guess145.txt"):
        p = os.path.join(d, f)
        if not os.path.exists(p):
            continue
        for line in open(p):
            m = re.match(r"([\w./+$?@-]+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line.split("//")[0].strip())
            if m:
                out.setdefault(int(m.group(2), 16), m.group(1).split("/")[-1])
    return out


def ranges():
    rows = list(csv.DictReader(open(os.path.join(B, "match69.csv"))))
    out = {}
    for i, r in enumerate(rows):
        a, n = int(r["b69_start"], 16), int(r["b69_size"])
        if r["new_start"]:
            out[a] = (n, int(r["new_start"], 16), int(r["new_start"], 16) + n)
            continue
        prev = next((rows[j] for j in range(i - 1, -1, -1) if rows[j]["new_start"]), None)
        nxt = next((rows[j] for j in range(i + 1, len(rows)) if rows[j]["new_start"]), None)
        lo = int(prev["new_start"], 16) + int(prev["b69_size"]) if prev else None
        hi = int(nxt["new_start"], 16) if nxt else None
        # several unmatched in a row share the gap: offset by the b69 distance from the gap start
        p69 = int(prev["b69_start"], 16) + int(prev["b69_size"]) if prev else a
        n69 = int(nxt["b69_start"], 16) if nxt else a + n
        out[a] = (n, lo, hi, p69, n69)
    return out


def disasm(exe, lo, hi, syms):
    p = subprocess.run(
        ["objdump", "-d", "--no-show-raw-insn", f"--start-address=0x{lo:x}", f"--stop-address=0x{hi:x}", exe],
        capture_output=True,
        text=True,
    )
    out = []
    for line in p.stdout.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+(.*)$", line)
        if not m:
            continue
        ins = re.sub(r"\s+", " ", m.group(2).split("<")[0]).strip()

        def name(mm):
            v = int(mm.group(0), 16)
            if v in syms:
                return syms[v]
            if lo <= v < hi:
                return f"L+{v - lo:x}"
            return "ADDR" if 0x400000 <= v < 0x600000 else mm.group(0)

        ins = re.sub(r"0x[0-9a-f]{6,8}\b", name, ins)
        out.append(ins)
    return out


EXE233 = os.path.join(REPO, "orig", "PS2_2_3_3", "mwccps2.exe")


def nr(ins):
    # stack slots and frame sizes move with the frame: mask them
    ins = re.sub(r"(-?0x[0-9a-f]+)?\(%esp(,[^)]*)?\)", "S(%esp)", ins)
    ins = re.sub(r"(subl|addl) \$0x[0-9a-f]+, %esp", r"\1 $F, %esp", ins)
    ins = re.sub(r"%[a-z]{2,3}\b", "%r", ins)
    return re.sub(r"L\+[0-9a-f]+", "L", ins)


PAD = re.compile(r"^(nop|leal \(%r(,%r)?\), %r|leal \(,?%r,?%r?\), %r|addb .*|int3)$")


def clean(lines):
    return [i for i in map(nr, lines) if not PAD.match(i)]


def ndiff(x, y):
    return sum(1 for l in difflib.unified_diff(x, y, n=0, lineterm="") if l[:1] in "+-" and l[:3] not in ("+++", "---"))


def new_range(va, r):
    if len(r) == 3:
        return r[1], r[2]
    n, glo, ghi, p69, n69 = r
    if n69 - p69 == n:
        return glo, ghi
    return glo + (va - p69), glo + (va - p69) + n + (ghi - glo) - (n69 - p69)


def compare_all():
    """b69 vs 011126 and 2.3.3 vs 011126 diff sizes (register-insensitive) per unmatched function"""
    import csv as _csv

    s69 = load_syms(os.path.join(os.path.dirname(B), "b69"))
    sN = load_syms(B)
    s233 = load_syms(os.path.join(os.path.dirname(B), "b233"))
    n233 = {v: k for k, v in s233.items()}
    sz233 = {
        int(r["start"], 16): int(r["size"])
        for r in _csv.DictReader(open(os.path.join(os.path.dirname(B), "b233", "functions.csv")))
    }
    rg = ranges()
    for va, r in sorted(rg.items()):
        if len(r) == 3:
            continue
        nm = s69.get(va, "?")
        lo, hi = new_range(va, r)
        x = clean(disasm(EXE69, va, va + r[0], s69))
        y = clean(disasm(EXEN, lo, hi, sN))
        d69 = ndiff(x, y)
        a3 = n233.get(nm) if nm != "?" else None
        d233 = "-"
        if a3 in sz233:
            z = clean(disasm(EXE233, a3, a3 + sz233[a3], s233))
            d233 = ndiff(z, y)
        print(f"{va:08X} {len(y):5d} b69 {d69:5d} 2.3.3 {d233:>5} {nm}", flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "--compare", action="store_true", help="diff sizes against b69 and 2.3.3 for every unmatched function"
    )
    ap.add_argument("addr", nargs="?")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--context", type=int, default=3)
    ap.add_argument("--noreg", action="store_true", help="ignore register names, local labels and nops")
    ap.add_argument("--new", help="011126 start,end (hex) when the gap holds several functions")
    a = ap.parse_args()
    if a.compare:
        compare_all()
        return
    rg = ranges()
    s69 = load_syms(os.path.join(os.path.dirname(B), "b69"))
    sN = load_syms(B)
    if a.list:
        for va, r in sorted(rg.items()):
            if len(r) == 5:
                n, lo, hi, p69, n69 = r
                print(
                    f"{va:08X} {n:6d} gap69 {n69 - p69:6d} gap011126 {hi - lo:6d} delta {hi - lo - (n69 - p69):+5d} {s69.get(va, '?')}"
                )
        return
    va = int(a.addr, 16)
    r = rg[va]
    n = r[0]
    if a.new:
        lo, hi = (int(x, 16) for x in a.new.split(","))
    elif len(r) == 3:
        lo, hi = r[1], r[2]
    else:
        _, glo, ghi, p69, n69 = r
        if n69 - p69 == n:
            lo, hi = glo, ghi
        else:
            lo, hi = glo + (va - p69), glo + (va - p69) + n + (ghi - glo) - (n69 - p69)
            print(f"note: the gap holds several b69 functions; guessing 011126 {lo:08X}..{hi:08X} (use --new)")
    x = disasm(EXE69, va, va + n, s69)
    y = disasm(EXEN, lo, hi, sN)
    if a.noreg:
        x, y = clean(x), clean(y)
    print(
        f"b69 {va:08X} ({n} bytes, {len(x)} insns) vs 011126 {lo:08X}..{hi:08X} ({hi - lo} bytes, {len(y)} insns): {s69.get(va, '?')}"
    )
    for line in difflib.unified_diff(x, y, "b69", "011126", n=a.context, lineterm=""):
        print(line)


if __name__ == "__main__":
    main()
