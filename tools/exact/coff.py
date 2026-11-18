"""Reader for the i386 COFF objects CodeWarrior for Windows writes (the
research harness's coff.py), plus bind_pro6() for the Pro 6/5.3 objects of
the b69 line."""

import re
import struct

REL_DIR32 = 0x06
REL_DIR32NB = 0x07
REL_REL32 = 0x14

SCN_CNT_CODE = 0x20
SCN_CNT_UNINIT = 0x80
SCN_LNK_INFO = 0x200
SCN_LNK_REMOVE = 0x800
SCN_MEM_WRITE = 0x80000000


class Section:
    def __init__(self, index, name, data, size, flags, relocs):
        self.index, self.name, self.data, self.size = index, name, data, size
        self.flags, self.relocs = flags, relocs

    @property
    def code(self):
        return bool(self.flags & SCN_CNT_CODE)

    @property
    def bss(self):
        return bool(self.flags & SCN_CNT_UNINIT)


class Symbol:
    def __init__(self, index, name, value, section, typ, cls):
        self.index, self.name, self.value, self.section = index, name, value, section
        self.type, self.cls = typ, cls

    @property
    def defined(self):
        return self.section > 0


class Object:
    def __init__(self, path):
        self.path = path
        d = open(path, "rb").read()
        mach, nsec, _, symp, nsym, opt, _ = struct.unpack_from("<HHIIIHH", d, 0)
        assert mach == 0x14C, f"{path}: not i386 COFF"
        strtab = symp + 18 * nsym
        self.symbols = {}
        i = 0
        while i < nsym:
            e = d[symp + 18 * i : symp + 18 * i + 18]
            if e[:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", e, 4)[0]
                name = d[strtab + off : d.index(b"\0", strtab + off)]
            else:
                name = e[:8].rstrip(b"\0")
            value, sec, typ, cls, naux = struct.unpack_from("<IhHBB", e, 8)
            self.symbols[i] = Symbol(i, name.decode("latin1"), value, sec, typ, cls)
            i += 1 + naux
        self.sections = []
        for s in range(nsec):
            h = 20 + opt + 40 * s
            name = d[h : h + 8].rstrip(b"\0").decode("latin1")
            if name.startswith("/"):
                off = int(name[1:])
                name = d[strtab + off : d.index(b"\0", strtab + off)].decode("latin1")
            _, _, size, raw, relp, _, nrel, _, flags = struct.unpack_from("<IIIIIIHHI", d, h + 8)
            data = b"\0" * size if flags & SCN_CNT_UNINIT else d[raw : raw + size]
            relocs = [struct.unpack_from("<IIH", d, relp + 10 * r) for r in range(nrel)]
            self.sections.append(Section(s + 1, name, data, size, flags, relocs))

    def section(self, index):
        return self.sections[index - 1]


def bind_pro6(o):
    """CW Pro 6 addresses a unit's statics through the section symbol
    (.data/.bss + offset), which the relocation check refuses as a second copy
    of writable state. The section symbol of each writable data section
    becomes the static `__b69sec<name>` (`<unit>/__b69sec.bss + offset`),
    which the b69/b52 name tables place at the original's data. Pro 6 also
    names string literals and some statics `_@N`; they become `__b69atN`.

    A switch's jump table in its own writable .data section, every slot a
    DIR32 relocation to the one function using it, is never written: it is
    marked read-only, so the check compares it against the original. (The
    research tree's tools/mwccps2/b69/sections.py.)"""
    for sym in o.symbols.values():
        m = re.fullmatch(r"_@(\d+)", sym.name)
        if m:
            sym.name = "__b69at" + m.group(1)
            continue
        if sym.section > 0 and sym.value == 0 and sym.cls == 3:
            try:
                sec = o.section(sym.section)
            except IndexError:
                continue
            if sym.name == sec.name and not sec.code and sec.name in (".data", ".bss"):
                sym.name = "__b69sec" + sec.name
    for sec in o.sections:
        if sec.code or sec.bss or not sec.flags & SCN_MEM_WRITE or sec.name != ".data" or not sec.relocs:
            continue
        if sec.size % 4 or len(sec.relocs) * 4 != sec.size:
            continue
        if sorted(r for r, _, _ in sec.relocs) != list(range(0, sec.size, 4)):
            continue
        if any(t != REL_DIR32 for _, _, t in sec.relocs):
            continue
        if len({o.symbols[si].name for _, si, _ in sec.relocs}) != 1:
            continue
        tsym = o.symbols[sec.relocs[0][1]]
        if not (tsym.section > 0 and o.section(tsym.section).code):
            continue
        sec.flags &= ~SCN_MEM_WRITE
