#include "compiler/mwcc_core.h"
#include "compiler/common.h"
#include "psp/BE.h"
#include "compiler/Alias.h"
#include "compiler/CError.h"
#include "compiler/objects.h"
#include "compiler/CParser.h"
#include "compiler/RegisterInfo.h"

// mwccpsp 3.0.1 PCodeUtilities.c (named by its assertions): emitting
// common instruction shapes.

#define OBJECT_FRAMEOFFSET(obj) (*(SInt32*)&((Object*)(obj))->u.var.offset) // +0x30

extern SInt16 data_00625E3E; // frame register
extern MPCode* func_00480420(int op, SInt16 nargs);
extern SInt32 func_0048FBE0(UInt16 op); // bytes the memory instruction accesses

inline MOperand reg_operand(SInt16 reg) {
    MOperand op;
    op.kind = 0;
    op.rclass = 0;
    op.reg = reg;
    return op;
}

inline MOperand fpr_operand(SInt16 reg) {
    MOperand op;
    op.kind = 0;
    op.rclass = 1;
    op.reg = reg;
    return op;
}

extern SInt16 data_00625E3E; // the frame register
extern MPCodeBlock* data_006246B4; // the block being generated

// 0x480230: the access just made may be misaligned: split a doubleword load
// or a word store into its left and right halves (unless it is relative to
// the frame or $gp, or is aligned enough)
#if !MWCCPS2 // mips.c has b210's version
void func_00480230(SInt16 align, void* unk) {
    MPCode* pc;
    MPCode* npc;
    SInt16 base;

    pc = data_006246B4->last;
    switch (pc->op) {
        case MACRO_OP(0x4A):
            base = pc->args[2].reg;
            if (base == data_00625E3E || base == 28 || align >= 8) {
                return;
            }
            pc->op = copts.littleendian ? MACRO_OP(0x4F) : MACRO_OP(0x4E);
            npc = func_00480420(copts.littleendian ? MACRO_OP(0x4E) : MACRO_OP(0x4F), 3);
            npc->flags = pc->flags;
            npc->args[0] = pc->args[0];
            npc->args[0].flags |= 1;
            npc->args[1] = pc->args[1];
            npc->args[2] = pc->args[2];
            npc->alias = pc->alias;
            insertpcodeafter(pc, npc);
            npc->args[1].value += 7;
            break;
        case MACRO_OP(0x55):
            base = pc->args[2].reg;
            if (base == data_00625E3E || base == 28 || align >= 4) {
                return;
            }
            pc->op = copts.littleendian ? MACRO_OP(0x5B) : MACRO_OP(0x5A);
            npc = func_00480420(copts.littleendian ? MACRO_OP(0x5A) : MACRO_OP(0x5B), 3);
            npc->flags = pc->flags;
            npc->args[0] = pc->args[0];
            npc->args[0].flags |= 1;
            npc->args[1] = pc->args[1];
            npc->args[2] = pc->args[2];
            npc->alias = pc->alias;
            insertpcodeafter(pc, npc);
            npc->args[1].value += 3;
            break;
        case MACRO_OP(0x5D):
            CError_FATAL(0x241);
            break;
    }
}
#endif

// branch_always: an unconditional branch ends the block.
void func_00480710(MPCodeLabel* label) {
    SInt16 op = 0x39;
    emitpcode(op, label);
    pcbranch(data_006246B4, label, 0);
    makepcblock();
}

extern MOperand func_004C3D90(SInt32 value); // an immediate operand

// dst = src op imm
void func_00480740(int op, SInt16 dst, SInt16 src, SInt32 imm) {
    MPCode* pc;

    pc = func_00480420(op, 3);
    pc->args[0].kind = 0;
    pc->args[0].rclass = 0;
    pc->args[0].reg = dst;
    pc->args[0].flags = 2;
    pc->args[1].kind = 0;
    pc->args[1].rclass = 0;
    pc->args[1].reg = src;
    pc->args[1].flags = 1;
    pc->args[2] = func_004C3D90(imm);
    appendpcode(data_006246B4, pc);
}

#if MWCC_CORE == MWCC_CORE_B011126
// 011126 (0x439A80, called by the four load and store helpers here: 0x4979EC,
// 0x497AF0, 0x497C4B, 0x497D0B): every access gets an alias (none without an
// object), from the offset as it is; the object's part only when the access
// is based on nothing or on the frame register being $sp (no datatype test).
// AliasOperand.c's func_0053BD10 is the same for the other helpers.
static struct Alias* b011126_mem_alias(Object* obj, SInt32 offset, SInt32 size, UInt16 base) {
    if (base != 0 && !(base == data_00625E3E && base == 29)) {
        return make_alias(obj, 0, 0);
    }
    return make_alias(obj, offset, size);
}
#endif

// Store register reg to obj+offset addressed from base.
MPCode* func_004808E0(SInt16 op, SInt16 reg, SInt16 base, Object* obj, SInt32 offset) {
    MPCode* pc;

    pc = func_00480420(op, 3);
    pc->args[0] = reg_operand(reg);
    pc->args[0].flags = 1;
    pc->args[1].kind = 4;
    MOP_SETOBJ(pc->args[1], obj);
    pc->args[1].value = offset;
    pc->args[2] = reg_operand(base);
    pc->args[2].flags = 1;
#if MWCC_CORE == MWCC_CORE_B011126
    pc->alias = b011126_mem_alias(obj, offset, func_0048FBE0(pc->op), pc->args[2].reg);
    if (0)
#endif
        if (obj) {
#if MWCCPS2 && MWCCPS2_BUILD == 119
            // b119: every object that is not a local off another base gets a sized alias
            if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
                pc->alias = make_alias(obj, 0, 0);
            } else {
                pc->alias =
                    make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
            }
#else
        if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
            pc->alias = make_alias(obj, 0, 0);
        } else if (obj->datatype == DLOCAL) {
            pc->alias = make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
        } else if (obj->datatype == DDATA) {
            pc->alias = make_alias(obj, 0, 0);
        } else {
            CError_FATAL(222);
        }
#endif
        }
    if (!pc->alias) {
        pc->flags |= 0x20;
    }
    appendpcode(data_006246B4, pc);
    return pc;
}

extern SInt16 data_00625E48; // move
#if MWCCPS2
extern SInt16 data_00625E74; // quadword move
#endif

// Load register reg from obj+offset addressed from base; a local that lives
// in a register is just moved.
void func_00480B40(SInt16 op, SInt16 reg, SInt16 base, Object* obj, SInt32 offset) {
    MPCode* pc;
    VarInfo* vi;
    SInt16 home;

    if (obj && obj->datatype == DLOCAL) {
        vi = Registers_GetVarInfo(obj);
    } else {
        vi = NULL;
    }
    if (vi && (home = vi->reg)) {
#if MWCCPS2
        // lq (0x467 macro, 0x1D1): a quadword local in a register
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        if (op == OPC(0x467) || op == OPC(0x1D1)) {
#else
#if MWCCPS2_BUILD == 119
        if ((int)op == PS2_OP(0x467) || (int)op == PS2_OP(0x1D1)) { // b119 compares as int
#else
        if (op == PS2_OP(0x467) || op == PS2_OP(0x1D1)) {
#endif
#endif
            emitpcode(data_00625E74, reg, home);
        } else {
            emitpcode(data_00625E48, reg, home);
        }
#else
        emitpcode(data_00625E48, reg, home);
#endif
#if MWCCPS2 && MWCCPS2_BUILD == 119
    } else { // b119 shares one epilogue
#else
        return;
    }
#endif

        pc = func_00480420(op, 3);
        pc->args[0] = reg_operand(reg);
        pc->args[0].flags = 2;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        if (op ==
            (UInt16)((copts.mips_isa >= 2 && (copts.fp_library & 2)) ? MACRO_OP(0x4B)
                                                                     : MACRO_OP(0x48))) {
#else
    if (op ==
        (UInt16)((copts.mips_isa >= 2 && (copts.fp_library & 2)) ? MACRO_OP(0x4B)
                                                                 : MACRO_OP(0x48))) {
#endif
            pc->args[0].flags |= 0x8000;
            if (copts.mips_option1A) {
                pc->args[0].flags |= 0x20;
            }
        }
        pc->args[1].kind = 4;
        MOP_SETOBJ(pc->args[1], obj);
        pc->args[1].value = offset;
        pc->args[2] = reg_operand(base);
        pc->args[2].flags = 1;
#if MWCC_CORE == MWCC_CORE_B011126
        pc->alias = b011126_mem_alias(obj, offset, func_0048FBE0(pc->op), pc->args[2].reg);
        if (0)
#endif
            if (obj) {
#if MWCCPS2 && MWCCPS2_BUILD == 119
                // b119: every object that is not a local off another base gets a sized alias
                if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
                    pc->alias = make_alias(obj, 0, 0);
                } else {
                    pc->alias =
                        make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
                }
#else
        if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
            pc->alias = make_alias(obj, 0, 0);
        } else if (obj->datatype == DLOCAL) {
            pc->alias = make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
        } else if (obj->datatype == DDATA) {
            pc->alias = make_alias(obj, 0, 0);
        } else {
            CError_FATAL(96);
        }
#endif
            }
        if (!pc->alias) {
            pc->flags |= 0x20;
        }
        if (obj && is_volatile_object(obj)) {
            pc->flags |= 0x80;
        }
        if (obj && is_const_object(obj)) {
            pc->flags |= 0x40;
        }
        appendpcode(data_006246B4, pc);
#if MWCCPS2 && MWCCPS2_BUILD == 119
    }
#endif
}

// Store floating-point register reg to obj+offset addressed from base.
MPCode* func_004807C0(SInt16 op, SInt16 reg, SInt16 base, Object* obj, SInt32 offset) {
    MPCode* pc;

    pc = func_00480420(op, 3);
    pc->args[0] = fpr_operand(reg);
    pc->args[0].flags = 1;
    pc->args[1].kind = 4;
    MOP_SETOBJ(pc->args[1], obj);
    pc->args[1].value = offset;
    pc->args[2] = reg_operand(base);
    pc->args[2].flags = 1;
#if MWCC_CORE == MWCC_CORE_B011126
    pc->alias = b011126_mem_alias(obj, offset, func_0048FBE0(pc->op), pc->args[2].reg);
    if (0)
#endif
        if (obj) {
#if MWCCPS2 && MWCCPS2_BUILD == 119
            // b119: every object that is not a local off another base gets a sized alias
            if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
                pc->alias = make_alias(obj, 0, 0);
            } else {
                pc->alias =
                    make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
            }
#else
        if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
            pc->alias = make_alias(obj, 0, 0);
        } else if (obj->datatype == DLOCAL) {
            pc->alias = make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
        } else if (obj->datatype == DDATA) {
            pc->alias = make_alias(obj, 0, 0);
        } else {
            CError_FATAL(261);
        }
#endif
        }
    if (!pc->alias) {
        pc->flags |= 0x20;
    }
    appendpcode(data_006246B4, pc);
    return pc;
}

// Load floating-point register reg from obj+offset addressed from base.
void func_00480A00(SInt16 op, SInt16 reg, SInt16 base, Object* obj, SInt32 offset) {
    MPCode* pc;

    pc = func_00480420(op, 3);
    pc->args[0].kind = 0;
    pc->args[0].rclass = 1;
    pc->args[0].reg = reg;
    pc->args[0].flags = 2;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    if (op ==
        (UInt16)((copts.mips_isa >= 2 && (copts.fp_library & 2)) ? MACRO_OP(0x4B)
                                                                 : MACRO_OP(0x48))) {
#else
    if (op ==
        (UInt16)((copts.mips_isa >= 2 && (copts.fp_library & 2)) ? MACRO_OP(0x4B)
                                                                 : MACRO_OP(0x48))) {
#endif
        pc->args[0].flags |= 0x8000;
        if (copts.mips_option1A) {
            pc->args[0].flags |= 0x20;
        }
    }
    pc->args[1].kind = 4;
    MOP_SETOBJ(pc->args[1], obj);
    pc->args[1].value = offset;
    pc->args[2].kind = 0;
    pc->args[2].rclass = 0;
    pc->args[2].reg = base;
    pc->args[2].flags = 1;
#if MWCC_CORE == MWCC_CORE_B011126
    pc->alias = b011126_mem_alias(obj, offset, func_0048FBE0(pc->op), pc->args[2].reg);
    if (0)
#endif
        if (obj) {
#if MWCCPS2 && MWCCPS2_BUILD == 119
            // b119: every object that is not a local off another base gets a sized alias
            if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
                pc->alias = make_alias(obj, 0, 0);
            } else {
                pc->alias =
                    make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
            }
#else
        if (obj->datatype == DLOCAL && pc->args[2].reg != data_00625E3E) {
            pc->alias = make_alias(obj, 0, 0);
        } else if (obj->datatype == DLOCAL) {
            pc->alias = make_alias(obj, offset - OBJECT_FRAMEOFFSET(obj), func_0048FBE0(pc->op));
        } else if (obj->datatype == DDATA) {
            pc->alias = make_alias(obj, 0, 0);
        } else {
            CError_FATAL(148);
        }
#endif
        }
    if (obj && is_volatile_object(obj)) {
        pc->flags |= 0x80;
    }
    if (obj && is_const_object(obj)) {
        pc->flags |= 0x40;
    }
    appendpcode(data_006246B4, pc);
}
