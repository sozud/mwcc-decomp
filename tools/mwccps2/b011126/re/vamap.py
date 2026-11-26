#!/usr/bin/env python3
"""Map b69 addresses to 3.0-011126 addresses: tools/mwccps2/b011126/vamap.json.

    vamap.py

- code: each b69 function's start from match69.csv (functions whose masked bytes
  are b69's), and every offset inside a matched function;
- data: the pointers matched functions hold at the same offsets (relocated dwords,
  b69 target -> 011126 target), plus an alignment of the two .data sections
  (difflib over 4-byte words with relocated words masked) for the rest.

Conflicting pointer pairs are reported; the alignment never overrides a pointer pair.
"""

import csv
import difflib
import json
import os
import struct
import sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
B = os.path.normpath(os.path.join(HERE, ".."))
REPO = os.path.normpath(os.path.join(B, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "mwccpsp"))
from pe import PE  # noqa: E402


def sect(pe, name):
    s = pe.section(name)
    return pe.base + s[1], s[2]


def main():
    p69 = PE(os.path.join(REPO, "orig", "PS2_3_0_3_b69", "mwccps2.exe"))
    pN = PE(os.path.join(REPO, "orig", "PS2_3_0_011126", "mwccps2.exe"))
    r69, rN = set(p69.relocations()), set(pN.relocations())
    rows = list(csv.DictReader(open(os.path.join(B, "match69.csv"))))

    code = {}
    pairs = defaultdict(Counter)
    calls = defaultdict(Counter)
    starts69 = {int(r["b69_start"], 16) for r in rows}
    for r in rows:
        if not r["new_start"]:
            continue
        a, b, n = int(r["b69_start"], 16), int(r["new_start"], 16), int(r["b69_size"])
        code[a] = b
        ca, cb = p69.read(a, n), pN.read(b, n)
        o = 0
        while o < n:
            if a + o in r69 and b + o in rN:
                x = struct.unpack_from("<I", ca, o)[0]
                y = struct.unpack_from("<I", cb, o)[0]
                pairs[x][y] += 1
                o += 4
                continue
            if ca[o] == 0xE8 and o + 5 <= n:
                # call rel32 (masked alike in both): pair the targets that are b69 function starts
                x = a + o + 5 + struct.unpack_from("<i", ca, o + 1)[0]
                y = b + o + 5 + struct.unpack_from("<i", cb, o + 1)[0]
                if x in starts69:
                    calls[x][y] += 1
            o += 1

    called = {}
    for x, c in calls.items():
        y, k = c.most_common(1)[0]
        if x not in code and (len(c) == 1 or k >= 2 * sum(c.values()) // 3):
            called[x] = y
    ptr = {}
    conflicts = 0
    for x, c in pairs.items():
        y, k = c.most_common(1)[0]
        if len(c) > 1:
            conflicts += 1
        ptr[x] = y

    # .data alignment, word by word
    d69, n69 = sect(p69, ".data")
    dN, nN = sect(pN, ".data")

    def words(pe, base, n, rel):
        out = []
        for o in range(0, n - 3, 4):
            out.append(b"R" if base + o in rel else bytes(pe.read(base + o, 4)))
        return out

    w69, wN = words(p69, d69, n69, r69), words(pN, dN, nN, rN)
    sm = difflib.SequenceMatcher(None, w69, wN, autojunk=False)
    blocks = [b for b in sm.get_matching_blocks() if b.size]
    data = {}
    for b in blocks:
        for i in range(b.size):
            data[d69 + 4 * (b.a + i)] = dN + 4 * (b.b + i)
    agree = sum(1 for x, y in ptr.items() if data.get(x) == y)
    disagree = sum(1 for x, y in ptr.items() if x in data and data[x] != y)

    out = {
        "functions": {f"{a:08X}": f"{b:08X}" for a, b in sorted(code.items())},
        "called": {f"{a:08X}": f"{b:08X}" for a, b in sorted(called.items())},
        "pointers": {f"{a:08X}": f"{b:08X}" for a, b in sorted(ptr.items())},
        "data_blocks": [[f"{d69 + 4 * b.a:08X}", f"{dN + 4 * b.b:08X}", 4 * b.size] for b in blocks],
    }
    json.dump(out, open(os.path.join(B, "vamap.json"), "w"), indent=0)
    print(
        f"functions {len(code)} (+{len(called)} unmatched placed by calls); pointer targets {len(ptr)} ({conflicts} with conflicting pairs); "
        f".data aligned {sum(b.size for b in blocks) * 4}/{n69} bytes in {len(blocks)} blocks; "
        f"pointers vs alignment: {agree} agree, {disagree} disagree"
    )


def load():
    """(translate(va) -> va or None) from vamap.json"""
    m = json.load(open(os.path.join(B, "vamap.json")))
    fn = {int(a, 16): int(b, 16) for a, b in m["functions"].items()}
    called = {int(a, 16): int(b, 16) for a, b in m.get("called", {}).items()}
    ptr = {int(a, 16): int(b, 16) for a, b in m["pointers"].items()}
    blocks = sorted((int(a, 16), int(b, 16), n) for a, b, n in m["data_blocks"])
    import bisect

    starts = [b[0] for b in blocks]
    bss = sorted(a for a in ptr if 0x54A000 <= a < 0x57F000)
    fstarts = sorted(fn)
    sizes = {}
    for r in csv.DictReader(open(os.path.join(B, "match69.csv"))):
        sizes[int(r["b69_start"], 16)] = int(r["b69_size"])

    def tr(va):
        if va in ptr:
            return ptr[va]
        if va in fn:
            return fn[va]
        if va in called:
            return called[va]
        i = bisect.bisect_right(fstarts, va) - 1
        if i >= 0 and va < fstarts[i] + sizes.get(fstarts[i], 0):
            return fn[fstarts[i]] + (va - fstarts[i])
        i = bisect.bisect_right(starts, va) - 1
        if i >= 0 and va < blocks[i][0] + blocks[i][2]:
            return blocks[i][1] + (va - blocks[i][0])
        if 0x50D000 <= va < 0x50F000 or 0x546000 <= va < 0x54A000 or 0x57F000 <= va < 0x580000:
            return va  # .rdata .exc .rsrc .idata .CRT .tls .edata: the same sizes in both
        if 0x54A000 <= va < 0x57F000:
            # .bss: the delta of the pointer targets around it, when both sides agree
            j = bisect.bisect_right(bss, va) - 1
            if 0 <= j and j + 1 < len(bss):
                lo, hi = bss[j], bss[j + 1]
                if ptr[lo] - lo == ptr[hi] - hi:
                    return va + ptr[lo] - lo
        return None

    return tr


if __name__ == "__main__":
    main()
