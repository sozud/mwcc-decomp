"""Names for mwccpsp, found from our own objects as the research harness does
before every full build (tools/mwccpsp/re/import_mweppc.py, then align.py):

 1. prove(): each function of ours (16 bytes or more) whose bytes, relocation
    slots masked, occur exactly once in the original's .text, right after the
    end of a function, is placed there, and every symbol it references is
    placed at the address the original's slot holds. A name placed at two
    addresses is dropped, and so are names sharing an address unless exactly
    one of them was placed by its own match.
 2. guess(): positional guesses. Between two placed functions of one object,
    k functions of ours pair in order with k function starts of the original;
    the first/last placed function extends outwards along function starts
    whose unit label (functions.csv, read from the original's assertion
    strings) is the same unit; then call targets pair in order between our
    call list and the original's (capstone) for functions with equal-length
    lists. Guesses only place a name for the comparison: a guessed function
    counts only if it matches exactly there.
"""

import struct

from coff import REL_DIR32, REL_REL32
from match import c_name, functions_of, pattern


def _after_end(text, i):
    if i < 6:
        return True
    b = text[i - 6 : i]
    return b[-1] in (0x90, 0xCC, 0xC3) or b[-3] == 0xC2 or b[-5] == 0xE9 or b[-2] == 0xEB or b[0:2] == b"\xff\x25"


def _qualified(sym, unit):
    n = c_name(sym.name)
    return f"{unit}/{n}" if sym.cls == 3 else n


def prove(objects, pe):
    """objects: [(Object, unit)] in object-file-name order. {name: address}."""
    _, tva, tvs, traw, _ = pe.section(".text")
    text = bytes(pe.data[traw : traw + tvs])
    tlo = pe.base + tva
    names = {}  # name -> {addr: [evidence]}

    def note(name, addr, why):
        names.setdefault(name, {}).setdefault(addr, []).append(why)

    for o, unit in objects:
        for sym, sec, start, end in functions_of(o):
            s = o.section(sec)
            body = s.data[start:end]
            while body.endswith(b"\x90"):
                body = body[:-1]
            if len(body) < 16:
                continue
            relocs = [(off - start, si, typ) for off, si, typ in s.relocs if start <= off < start + len(body)]
            masked = {r[0] + k for r in relocs for k in range(4)}
            hits = [m.start() for m in pattern(body, masked).finditer(text) if _after_end(text, m.start())]
            if len(hits) != 1:
                continue
            at = tlo + hits[0]
            fname = _qualified(sym, unit)
            note(fname, at, "self")
            for off, si, typ in relocs:
                target = o.symbols[si]
                if target.name.startswith(("$", "@", ".")) or (
                    target.cls == 3 and target.defined and target.value == 0 and target.name.startswith(".")
                ):
                    continue  # section symbols / literals: anonymous
                if target.cls == 3 and target.defined and o.section(target.section).name.startswith("."):
                    if target.name == o.section(target.section).name:
                        continue
                addend = struct.unpack_from("<i", s.data, start + off)[0]
                if typ == REL_DIR32:
                    if addend != 0:
                        continue  # a field offset: the address isn't the symbol's
                    addr = struct.unpack_from("<I", text, hits[0] + off)[0]
                elif typ == REL_REL32:
                    addr = (at + off + 4 + struct.unpack_from("<i", text, hits[0] + off)[0] - addend) & 0xFFFFFFFF
                else:
                    continue
                note(_qualified(target, unit), addr, "ref")

    good = {n: a for n, a in names.items() if len(a) == 1}
    by_addr = {}
    for n, addrs in good.items():
        by_addr.setdefault(next(iter(addrs)), []).append(n)
    for addr, ns in by_addr.items():
        if len(ns) < 2:
            continue
        selfs = [n for n in ns if "self" in good[n][addr]]
        keep = selfs[0] if len(selfs) == 1 else None
        for n in ns:
            if n != keep:
                del good[n]
    return {n: next(iter(a)) for n, a in good.items()}


def _original_calls(text, lo, start, end):
    import capstone

    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [
        int(i.op_str, 16)
        for i in cs.disasm(text[start - lo : end - lo], start)
        if i.mnemonic == "call" and i.bytes[0] == 0xE8
    ]


def guess(objects, pe, functions, symbols, res):
    """functions: [(start, size, unit label)] of the original. symbols: the
    proven names (manual ones included). {name: address} of the guesses."""

    def named_address(key):
        m = res.addr_name.search(key.split("/")[-1])
        return int(m.group(1), 16) if m else None

    starts = sorted(f[0] for f in functions)
    unit_label = {f[0]: f[2].rstrip("?") for f in functions}
    ends = {f[0]: f[0] + f[1] for f in functions}
    index = {a: i for i, a in enumerate(starts)}
    proven_addrs = set(symbols.values())
    # positions never hand out an address a func_XXXXXXXX name already has
    known_addrs = proven_addrs | {named_address(c_name(f[0].name)) for o, _ in objects for f in functions_of(o)} - {
        None
    }

    guesses = {}
    for o, unit in objects:
        # object order: by section, then offset
        funcs = sorted(functions_of(o), key=lambda f: (f[1], f[2]))
        names = []
        for sym, sec, st, en in funcs:
            n = c_name(sym.name)
            key = f"{unit}/{n}" if sym.cls == 3 else n
            a = symbols.get(key, symbols.get(n) if sym.cls != 3 else None)
            names.append((key, a if a is not None else named_address(key)))
        anchors = [(i, a) for i, (_, a) in enumerate(names) if a is not None and a in index]
        for (i1, a1), (i2, a2) in zip(anchors, anchors[1:]):
            between_obj = names[i1 + 1 : i2]
            between_exe = starts[index[a1] + 1 : index[a2]]
            if a2 > a1 and between_obj and len(between_obj) == len(between_exe):
                for (n, _), addr in zip(between_obj, between_exe):
                    if addr not in known_addrs:
                        guesses.setdefault(n, set()).add(addr)
        if anchors:
            i, a = anchors[0]
            j = index[a] - 1
            while i > 0 and j >= 0 and unit_label.get(starts[j]) == unit and starts[j] not in known_addrs:
                i -= 1
                guesses.setdefault(names[i][0], set()).add(starts[j])
                j -= 1
            i, a = anchors[-1]
            j = index[a] + 1
            while (
                i + 1 < len(names)
                and j < len(starts)
                and unit_label.get(starts[j]) == unit
                and starts[j] not in known_addrs
            ):
                i += 1
                guesses.setdefault(names[i][0], set()).add(starts[j])
                j += 1

    # call targets: our call list (names, in order) against the original's
    _, tva, tvs, traw, _ = pe.section(".text")
    text = bytes(pe.data[traw : traw + tvs])
    lo = pe.base + tva
    ours = {}
    for o, unit in objects:
        for sym, sec, st, en in functions_of(o):
            n = c_name(sym.name)
            s = o.section(sec)
            calls = []
            for off, si, typ in sorted(s.relocs):
                if st <= off < en and typ == REL_REL32 and off >= 1 and s.data[off - 1] == 0xE8:
                    t = o.symbols[si]
                    cn = c_name(t.name)
                    calls.append(f"{unit}/{cn}" if t.cls == 3 else cn)
            ours[f"{unit}/{n}" if sym.cls == 3 else n] = calls

    def addr_of(k):
        if k in symbols:
            return symbols[k]
        if named_address(k) is not None:
            return named_address(k)
        g = guesses.get(k)
        return next(iter(g)) if g and len(g) == 1 else None

    for _ in range(6):
        added = 0
        for k, calls in ours.items():
            a = addr_of(k)
            if a is None or a not in ends or not calls:
                continue
            orig = _original_calls(text, lo, a, ends[a])
            if len(orig) != len(calls):
                continue
            if any(addr_of(c) is not None and addr_of(c) != x for c, x in zip(calls, orig)):
                continue
            for c, x in zip(calls, orig):
                if addr_of(c) is None and x not in proven_addrs and x in ends:
                    guesses.setdefault(c, set()).add(x)
                    added += 1
        if not added:
            break

    taken = {}
    for n, addrs in guesses.items():
        if len(addrs) == 1:
            taken.setdefault(next(iter(addrs)), []).append(n)
    out = {}
    for addr, ns in sorted(taken.items()):
        if len(ns) == 1 and ns[0] not in symbols and named_address(ns[0]) is None:
            out[ns[0]] = addr
    return out
