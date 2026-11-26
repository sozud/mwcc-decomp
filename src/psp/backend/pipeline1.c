#include "compiler/mwcc_core.h"
#if MWCCPS2
#include "compiler/common.h"
#include "psp/BE.h"
#include "compiler/CError.h"

// mwccps2 b210 only (0x556230..0x5568B0, between Coloring's spill code and
// StoreLoad301.c): the R5900 "pipeline 1" pass. In a block whose only
// pipeline-sensitive instructions are of the first kind (and more than one of
// them), it moves alternate ones to their pipeline-1 variants (mult -> mult1,
// div -> div1, mflo -> mflo1, ...), following the HI/LO dependences.

extern MPCodeBlock* data_00624FD0; // first block
// the three instructions that start a pipeline-1 candidate: CodeGen.c's
// opcode globals (set per function), one name each so the native build reads
// the copies CodeGen writes (read unsigned here)
extern UInt16 data_00625E42;
extern UInt16 data_00625CE0;
extern UInt16 data_00625E4A;
#define ps2_pipeop1_637322 data_00625E42
#define ps2_pipeop2_6371D0 data_00625CE0
#define ps2_pipeop3_63732A data_00625E4A
extern void freeoheap(void);
extern void ps2_peepinit_4BBC10(void); // Peephole.c: the per-class def tables
extern void func_004AFEB0(MPCodeBlock* block); // Peephole.c: uses learn their defining instruction

#if MWCCPS2_BUILD == 119
// b119 (0x554B40..0x554DAF): a smaller pass. No per-block census: every
// block's candidates alternate between the pipelines; only mult/multu and the
// candidate opcode in ps2_pipeop1 have pipeline-1 forms, and after a move only
// the first mflo and the first mfhi reading its result follow.
static int pipeof(MOperand* op) { // 0x554D00
    MPCode* def;
    MOperand* a;
    int i;
    int r;

    def = (MPCode*)MOP_OBJ(*op);
    if (def->op == 0xBA) {
        return -1;
    }
    if (def->op == PS2_OP(0x1DB) || def->op == PS2_OP(0x1DC) || def->op == ps2_pipeop1_637322) {
        return 0;
    }
    if (def->op == PS2_OP(0x202) ||
        def->op == PS2_OP(0x203) ||
        def->op == PS2_OP(0x1D3) ||
        def->op == PS2_OP(0x1D4) ||
        def->op == PS2_OP(0x440)) {
        return 1;
    }
    for (i = 0, a = def->args; i < def->argcount; i++, a++) {
        if (a->kind == 0 && (a->flags & 1)) {
            r = pipeof(a);
            if (r >= 0) {
                return r;
            }
        }
    }
    return -1;
}

static void tohilo1(MPCode* pc) { // the HI/LO operands name pipeline 1's pair
    int i;
    MOperand* o;

    for (i = 0, o = pc->args; i < pc->argcount; i++, o++) {
        if (o->rclass == 2 && o->kind == 0 && (SInt32)o->reg == 0) {
            o->reg = 1;
        }
    }
}

static void topipe1(MPCode* pc) { // 0x554C80
    if (pc->op == PS2_OP(0x1DB)) {
        pc->op = PS2_OP(0x202);
    } else if (pc->op == PS2_OP(0x1DC)) {
        pc->op = PS2_OP(0x203);
    } else if (pc->op == ps2_pipeop1_637322) {
        pc->op = PS2_OP(0x440);
    } else {
        CError_FATAL(88);
    }
    tohilo1(pc);
}

static void pipeline1block(MPCodeBlock* block) {
    MPCode* pc;
    MPCode* next;
    int a;
    int b;
    Boolean skip = 0xFF;

    for (pc = block->first; pc; pc = pc->next) {
        if (pc->op != ps2_pipeop1_637322 &&
            pc->op != ps2_pipeop2_6371D0 &&
            pc->op != ps2_pipeop3_63732A) {
            continue;
        }
        if (skip) {
            skip = 0;
            continue;
        }
        a = pipeof(&pc->args[1]);
        b = pipeof(&pc->args[2]);
        if (a == -1 && b == -1) {
            if (skip) {
                continue;
            }
            topipe1(pc);
            for (next = pc->next; next; next = next->next) {
                if (next->op == 0xA8 && (MPCode*)MOP_OBJ(next->args[1]) == pc) {
                    next->op = PS2_OP(0x1FA);
                    tohilo1(next);
                    break;
                }
            }
            for (next = pc->next; next; next = next->next) {
                if (next->op == 0xA9 && (MPCode*)MOP_OBJ(next->args[1]) == pc) {
                    next->op = PS2_OP(0x1FB);
                    tohilo1(next);
                    break;
                }
            }
            skip = 1;
        } else if (a == 1 || b == 1) {
            topipe1(pc);
            skip = 1;
        }
    }
}
#else
// which pipeline the value of a register operand comes from: 0, 1, or -1 if
// none can be told
static int pipeof(MOperand* op) {
    MPCode* def;
    MOperand* a;
    int i;
    int r;

    def = (MPCode*)MOP_OBJ(*op);
    switch (def->op) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0xBA):
#else
        case 0xBA:
#endif
            return -1;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x7C):
        case OPC(0x7D):
        case OPC(0xA8):
        case OPC(0xA9):
        case OPC(0xB2):
        case OPC(0xB3):
        case OPC(0xB6):
        case OPC(0xB7):
        case OPC(0x1D5):
        case OPC(0x1D6):
        case OPC(0x1DB):
        case OPC(0x1DC):
        case OPC(0x43F):
        case OPC(0x469):
#else
        case 0x7C:
        case 0x7D:
        case 0xA8:
        case 0xA9:
        case 0xB2:
        case 0xB3:
        case 0xB6:
        case 0xB7:
        case PS2_OP(0x1D5):
        case PS2_OP(0x1D6):
        case PS2_OP(0x1DB):
        case PS2_OP(0x1DC):
        case PS2_OP(0x43F):
        case PS2_OP(0x469):
#endif
            return 0;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x1D3):
        case OPC(0x1D4):
        case OPC(0x1D7):
        case OPC(0x1D8):
        case OPC(0x1FA):
        case OPC(0x1FB):
        case OPC(0x1FD):
        case OPC(0x1FE):
        case OPC(0x202):
        case OPC(0x203):
#else
        case PS2_OP(0x1D3):
        case PS2_OP(0x1D4):
        case PS2_OP(0x1D7):
        case PS2_OP(0x1D8):
        case PS2_OP(0x1FA):
        case PS2_OP(0x1FB):
        case PS2_OP(0x1FD):
        case PS2_OP(0x1FE):
        case PS2_OP(0x202):
        case PS2_OP(0x203):
#endif
            return 1;
    }
    for (i = 0, a = def->args; i < def->argcount; i++, a++) {
        if (a->kind == 0 && (a->flags & 1)) {
            r = pipeof(a);
            if (r >= 0) {
                return r;
            }
        }
    }
    return -1;
}

// the instruction's pipeline-1 variant
static void topipe1(MPCode* pc) {
    int i;
    MOperand* op;

    switch (pc->op) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x1DB):
#else
        case PS2_OP(0x1DB):
#endif
            pc->op = PS2_OP(0x202);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x1DC):
#else
        case PS2_OP(0x1DC):
#endif
            pc->op = PS2_OP(0x203);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x1D5):
#else
        case PS2_OP(0x1D5):
#endif
            pc->op = PS2_OP(0x1D7);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x1D6):
#else
        case PS2_OP(0x1D6):
#endif
            pc->op = PS2_OP(0x1D8);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0xA8):
#else
        case 0xA8:
#endif
            pc->op = PS2_OP(0x1FA);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0xA9):
#else
        case 0xA9:
#endif
            pc->op = PS2_OP(0x1FB);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0xB2):
#else
        case 0xB2:
#endif
            pc->op = PS2_OP(0x1FD);
            break;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0xB3):
#else
        case 0xB3:
#endif
            pc->op = PS2_OP(0x1FE);
            break;
#if MWCCPS2_BUILD >= 198
        case 0x7C:
            pc->op = PS2_OP(0x1D3);
            break;
        case PS2_OP(0x43F):
            pc->op = PS2_OP(0x440);
            break;
#else
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x7C):
#else
        case 0x7C:
        case PS2_OP(0x43F): // b145 has no 0x440 yet
#endif
            pc->op = PS2_OP(0x1D3);
            break;
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case OPC(0x43F):
            pc->op = 0x440;
            break;
        case OPC(0x7D):
#else
        case 0x7D:
#endif
            pc->op = PS2_OP(0x1D4);
            break;
        default:
            CError_FATAL(118);
    }
    for (i = 0, op = pc->args; i < pc->argcount; i++, op++) {
        if (op->rclass == 2 && op->kind == 0 && (SInt32)op->reg == 0) {
            op->reg = 1;
        }
    }
}

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
// b69 (0x4A6A30): the pipeline of a register operand's value: its defining
// instruction's, or the first known one among that instruction's inputs
static int pipeof69(MOperand* op) {
    MPCode* def;
    MOperand* a;
    int i;
    int r;

    def = (MPCode*)MOP_OBJ(*op);
    if (def->op == OPC(0xBA)) {
        return -1;
    }
    if (def->op == OPC(0x1DB) || def->op == OPC(0x1DC) || def->op == ps2_pipeop1_637322) {
        return 0;
    }
    if (def->op == OPC(0x202) ||
        def->op == OPC(0x203) ||
        def->op == OPC(0x1D3) ||
        def->op == OPC(0x1D4) ||
        def->op == OPC(0x440)) {
        return 1;
    }
    for (i = 0, a = def->args; i < def->argcount; i++, a++) {
        if (a->kind == 0 && (a->flags & 1)) {
            r = pipeof69(a);
            if (r >= 0) {
                return r;
            }
        }
    }
    return -1;
}

// b69 (0x4A6B60, pipeline1block): no census of the block. The first candidate
// stays in pipeline 0 and each converted one skips the next; a candidate
// whose inputs come from no known pipeline moves with its mfhi and mflo, one
// with an input from pipeline 1 moves alone.
#endif
static void pipeline1block(MPCodeBlock* block) {
    MPCode* pc;
    MPCode* next;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    MOperand* o;
#else
    int n0;
    int n1;
    int nhilo;
    int nwait;
#endif
    int a;
    int b;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    int i;
    UInt8 skip;
#else
    UInt16 op;
    Boolean skip; // also the scan's keep-going flag
#endif

#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
    n0 = 0;
    n1 = 0;
    nhilo = 0;
    nwait = 0;
#endif
    skip = 0xFF;
    for (pc = block->first; pc; pc = pc->next) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        if (pc->op != ps2_pipeop1_637322 &&
            pc->op != ps2_pipeop2_6371D0 &&
            pc->op != ps2_pipeop3_63732A) {
#else
        switch (pc->op) {
            case 0x7C:
            case 0x7D:
            case 0xB6:
            case 0xB7:
            case PS2_OP(0x1D5):
            case PS2_OP(0x1D6):
            case PS2_OP(0x1DB):
            case PS2_OP(0x1DC):
            case PS2_OP(0x43F):
            case PS2_OP(0x469):
                n0++;
                break;
            case PS2_OP(0x1D3):
            case PS2_OP(0x1D4):
            case PS2_OP(0x1D7):
            case PS2_OP(0x1D8):
            case PS2_OP(0x202):
            case PS2_OP(0x203):
#endif
#if MWCCPS2_BUILD >= 198
            case PS2_OP(0x440):
#endif
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
                n1++;
                break;
            case 0xA8:
            case 0xA9:
                if (!n0) {
                    nwait++;
                }
                break;
            case PS2_OP(0x1FA):
            case PS2_OP(0x1FB):
                if (!n1) {
                    nwait++;
                }
                break;
            case 0xB2:
            case 0xB3:
            case PS2_OP(0x1FD):
            case PS2_OP(0x1FE):
                nhilo++;
                break;
        }
    }
    if (n0 <= 1 || n1 || nhilo || nwait) {
        return;
    }

    for (pc = block->first; pc; pc = pc->next) {
        if (pc->op != ps2_pipeop1_637322 &&
            pc->op != ps2_pipeop2_6371D0 &&
            pc->op != ps2_pipeop3_63732A) {
#endif
            continue;
        }
#if MWCC_CORE == MWCC_CORE_B233
        // 2.3.3 (0x4A0F30): only the first candidate stays in pipeline 0 for
        // certain; after a conversion one with inputs from no known pipeline
        // stays too, one with an input from pipeline 1 still moves
        if ((signed char)skip < 0) {
            skip = 0;
            continue;
        }
        a = pipeof69(&pc->args[1]);
        b = pipeof69(&pc->args[2]);
        // (no mflo/mfhi follows the conversion: 0x4A0EB0 changes the one
        // instruction)
        if (a == -1 && b == -1) {
            if (!skip) {
                topipe1(pc);
                skip = 1;
            }
            continue;
        }
#else
        if (skip) {
            skip = 0;
            continue;
        }
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B233
        a = pipeof69(&pc->args[1]);
        b = pipeof69(&pc->args[2]);
#endif
        if (a == -1 && b == -1) {
#else
        a = pipeof(&pc->args[1]);
        b = pipeof(&pc->args[2]);
#endif
#if MWCCPS2_BUILD < 198
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
            // b145: an operand from pipeline 1 moves only this instruction
            if (a != -1 || b != -1) {
                if (a == 1 || b == 1) {
#endif
                    topipe1(pc);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                    for (next = pc->next; next; next = next->next) {
                        if (next->op == OPC(0xA8) && (MPCode*)MOP_OBJ(next->args[1]) == pc) {
                            next->op = 0x1FA;
                            for (i = 0, o = next->args; i < next->argcount; i++, o++) {
                                if (o->rclass == 2 && o->kind == 0 && (SInt32)o->reg == 0) {
                                    o->reg = 1;
#else
            skip = 1;
#endif
                                }
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
                            }
                            else if (!skip) {
#endif
#else
        if ((a == -1 && b == -1) || a == 1 || b == 1) {
#endif
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
                                skip = 1;
                                topipe1(pc);
                                for (next = pc->next; next && skip; next = next->next) {
                                    op = 0;
                                    switch (next->op) {
                                        case 0xA8:
                                            if ((MPCode*)MOP_OBJ(next->args[1]) == pc) {
                                                op = PS2_OP(0x1FA);
#endif
                                            }
                                            break;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
                                        case 0xA9:
                                            if ((MPCode*)MOP_OBJ(next->args[1]) == pc) {
                                                op = PS2_OP(0x1FB);
#endif
                                            }
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
                                            break;
                                        case PS2_OP(0x1D5):
                                            if ((MPCode*)MOP_OBJ(next->args[3]) == pc) {
                                                op = PS2_OP(0x1D7);
#endif
                                            }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                                            for (next = pc->next; next; next = next->next) {
                                                if (next->op == OPC(0xA9) &&
                                                    (MPCode*)MOP_OBJ(next->args[1]) == pc) {
                                                    next->op = 0x1FB;
#else
break;
case PS2_OP(0x1D6):
    if ((MPCode*)MOP_OBJ(next->args[3]) == pc) {
        op = PS2_OP(0x1D8);
    }
    break;
case 0x7C:
case 0x7D:
case 0xB2:
case 0xB3:
case 0xB6:
case 0xB7:
case PS2_OP(0x43F):
    skip = 0;
    break;
    }
    if (op) {
        MOperand* o;
        int i;

        next->op = op;
#endif
                                                    for (i = 0, o = next->args; i < next->argcount;
                                                         i++, o++) {
                                                        if (o->rclass == 2 &&
                                                            o->kind == 0 &&
                                                            (SInt32)o->reg == 0) {
                                                            o->reg = 1;
                                                        }
                                                    }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                                                    break;
#endif
                                                }
                                            }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                                            skip = 1;
                                    }
                                    else if (a == 1 || b == 1) {
                                        topipe1(pc);
#endif
                                        skip = 1;
                                    }
                                }
                            }
#endif // MWCCPS2_BUILD == 119

void pipeline1(Object* func) {
    MPCodeBlock* block;

    ps2_peepinit_4BBC10();
    for (block = data_00624FD0; block; block = block->next) {
        if (block->pcodecount) {
            func_004AFEB0(block);
            pipeline1block(block);
        }
    }
    freeoheap();
}
#endif
