#include "compiler/mwcc_core.h"
// mwccpsp 3.0.1: BE_LoopInvariants (0x4B3090..0x4B35D0), the MIPS back
// end's loop-invariant code motion over LoopDetection301.c's loop tree.
// An instruction moves to its loop's preheader when every register it reads
// is defined nowhere in the loop, it is its registers' only definition there,
// they are not live out of the preheader, and (for memory reads) no store in
// the loop may alias it. Loops with calls are left alone.
#include "compiler/common.h"
#include "compiler/Alias.h"
#include "compiler/CompilerTools.h"
#include "compiler/objects.h"
#include "psp/BE.h"
#include "psp/Loop301.h"

extern SInt32 data_00625A98[7]; // the next virtual register of each class
extern SInt32 data_006250A8; // set when code moved
extern void func_004C34A0(void); // liveness

SInt32* data_005F3D8C[7]; // definitions in the loop, per class and register
SInt32* data_005F3D70[7]; // uses in the loop, per class and register

#if MWCCPS2 // b210 counts the operands down with a post-decrement test
#define FOR_ARGS(n, op, pc) for (n = (pc)->argcount, op = (pc)->args; n--; op++)
#else
#define FOR_ARGS(n, op, pc) for (n = (pc)->argcount, op = (pc)->args; n; n--, op++)
#endif

// 0x4B34F0: counts the loop's definitions and uses of every register.
#if MWCCPS2
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
BE_NOINLINE // b210 calls it (0x4BE1F0)
#else
__declspec(noinline) // b210 calls it (0x4BE1F0)
#endif
#endif
    static void countregs(Loop301* loop) {
    char rclass;
    int i;
    BlockList301* bl;
    MPCode* pc;
    MOperand* op;
    int n;

    for (rclass = 0; rclass < 7; rclass++) {
        for (i = 0; i < data_00625A98[rclass]; i++) {
            data_005F3D8C[rclass][i] = 0;
            data_005F3D70[rclass][i] = 0;
        }
    }
    for (bl = loop->blocks; bl; bl = bl->next) {
        for (pc = bl->block->first; pc; pc = pc->next) {
            FOR_ARGS(n, op, pc) {
                if (op->kind == 0) {
                    if (op->flags & 2) {
                        data_005F3D8C[op->rclass][op->reg]++;
                    }
                    if (op->flags & 1) {
                        data_005F3D70[op->rclass][op->reg]++;
                    }
                }
            }
        }
    }
}

// 0x4B33E0
static int canmove(MPCode* pc, Loop301* loop) {
    UInt32 flags;
    MOperand* op;
    int n;

    flags = pc->flags;
    if (flags & 0x140D8D) {
        return 0;
    }
    op = pc->args;
    n = pc->argcount;
    while (n--) {
        if (op->kind == 0 && (op->flags & 2)) {
            if (data_005F3D8C[op->rclass][op->reg] > 1) {
                return 0;
            }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
            if ((data_0062541C[op->rclass].blocks[loop->preheader->number].liveout[op->reg >> 5] >>
                 (op->reg & 31)) &
                1) {
#else
            if ((data_0062541C[op->rclass].blocks[loop->preheader->number].liveout[op->reg >> 5] >>
                 (op->reg & 31)) &
                1) {
#endif
                return 0;
            }
        }
        op++;
    }
    if (pc->block != loop->header) {
        if (flags & 0x10000) {
            return 0;
        }
        if ((flags & 0x60006) && (flags & 0x20)) {
            return 0;
        }
    }
    return 1;
}

// 0x4B32C0: moves pc to the end of the preheader (before its branch).
#if MWCCPS2
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
BE_NOINLINE // b210 calls it (0x4BDFC0)
#else
__declspec(noinline) // b210 calls it (0x4BDFC0)
#endif
#endif
    static void movetopreheader(MPCode* pc, Loop301* loop) {
    MPCodeBlock* pre;
    MOperand* op;
    int n;

    pre = loop->preheader;
    deletepcode(pc);
    if (pre->last && (pre->last->flags & 1)) {
        insertpcodebefore(pre->last, pc);
    } else {
        appendpcode(pre, pc);
    }
    FOR_ARGS(n, op, pc) {
        if (op->kind == 0) {
            if (op->flags & 2) {
                data_005F3D8C[op->rclass][op->reg]--;
            }
            if (op->flags & 1) {
                data_005F3D70[op->rclass][op->reg]--;
            }
        }
    }
    FOR_ARGS(n, op, pc) {
        if (op->kind == 0) {
            if ((op->flags & 2) && data_005F3D70[op->rclass][op->reg] > 0) {
                loop->pressure[op->rclass]++;
            }
            if ((op->flags & 1) && data_005F3D70[op->rclass][op->reg] == 0) {
                loop->pressure[op->rclass]--;
            }
        }
    }
    data_006250A8 = 1;
}

#if MWCC_CORE == MWCC_CORE_B233
#include "compiler/types.h"
#include "compiler/CodeGen.h"
// 2.3.3's memory test for a load (0x4A6058..0x4A6090, 0x4A5FA0): no alias
// analysis; a pointer load never moves, a load of an object stays when a
// store in the loop names the object or goes through a pointer while the
// object may be aliased (0x432F50)
static Object* b233_li_obj(MPCode* pc) {
    if (pc->argcount > 1 && pc->args[1].kind == 4) {
        return (Object*)MOP_OBJ(pc->args[1]);
    }
    return NULL;
}

static Boolean b233_li_mayalias(Object* obj) {
    VarInfo* info;
    Type* type;

    if (!obj || (obj->datatype != DDATA && obj->datatype != DLOCAL)) {
        return FALSE;
    }
    info = obj->datatype == DLOCAL ? obj->u.var.info : NULL;
    if (!info || info->noregister) {
        return TRUE;
    }
    if (!(type = obj->type)) {
        return FALSE;
    }
    return type->type == TYPEARRAY ||
        type->type == TYPESTRUCT ||
        type->type == TYPECLASS ||
        (type->type == TYPEMEMBERPOINTER && type->size == 12);
}

static int b233_li_loadstays(MPCode* pc, Loop301* loop) {
    BlockList301* bl;
    MPCode* q;
    Object* obj;

    if (pc->flags & 0x20) {
        return 1;
    }
    obj = b233_li_obj(pc);
    for (bl = loop->blocks; bl; bl = bl->next) {
        for (q = bl->block->first; q; q = q->next) {
            if (q->flags & 0x40004) {
                if (q->flags & 0x20) {
                    if (b233_li_mayalias(obj)) {
                        return 1;
                    }
                } else if (b233_li_obj(q) == obj) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
#endif

// 0x4B3130
static void moveinvariants(Loop301* loop) {
    BlockList301* bl;
    BlockList301* bl2;
    MPCode* pc;
    MPCode* next;
    MPCode* q;
    MOperand* op;
    int n;
    int invariant;
    int hascall;
    int changed;

    if (!loop->preheader) {
        return;
    }
    hascall = 0;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
    // b69 (0x4ABF60): also leaves alone a loop of more than 200 instructions
    // (2.3.3, 0x4A62C0: more than 100)
    // (those without pcode flag 0x10 counted)
    n = 0;
#endif
#endif
    for (bl = loop->blocks; bl; bl = bl->next) {
        for (pc = bl->block->first; pc; pc = pc->next) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
            if (!(pc->flags & 0x10)) {
                n++;
            }
#endif
#endif
            if (pc->flags & 8) {
                hascall = 1;
            }
        }
    }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE == MWCC_CORE_B52
    if (hascall) {
#elif MWCC_CORE == MWCC_CORE_B233
    if (hascall || n > 100) {
#else
    if (hascall || n > 200) {
#endif
#else
    if (hascall) {
#endif
        return;
    }
    countregs(loop);
    do {
        changed = 0;
        for (bl = loop->blocks; bl; bl = bl->next) {
            for (pc = bl->block->first; pc; pc = next) {
                next = pc->next;
                invariant = 1;
                FOR_ARGS(n, op, pc) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                    if (op->kind == 0 &&
                        (op->flags & 1) &&
                        data_005F3D8C[op->rclass][op->reg] > 0) {
#else
                    if (op->kind == 0 &&
                        (op->flags & 1) &&
                        data_005F3D8C[op->rclass][op->reg] > 0) {
#endif
                        invariant = 0;
                        break;
                    }
                }
#if MWCC_CORE == MWCC_CORE_B233
                if (invariant && (pc->flags & 0x20002) && b233_li_loadstays(pc, loop)) {
                    invariant = 0;
                }
                if (0) {
#else
                if (invariant && (pc->flags & 0x20002)) {
#endif
                    for (bl2 = loop->blocks; bl2 && invariant; bl2 = bl2->next) {
                        for (q = bl2->block->first; q; q = q->next) {
                            if ((q->flags & 0x40004) && may_alias((PCode*)pc, (PCode*)q)) {
                                invariant = 0;
                                break;
                            }
                        }
                    }
                }
                if (invariant && canmove(pc, loop)) {
                    movetopreheader(pc, loop);
                    changed = 1;
                }
            }
        }
    } while (changed);
}

// 0x4B3090
void BE_LoopInvariants(Object* func) {
    char rclass;

    data_006250A8 = 0;
    if (func_0054AB90(0)) {
        gather_alias_info();
        func_004C34A0();
        func_0054AAB0();
        for (rclass = 0; rclass < 7; rclass++) {
            data_005F3D8C[rclass] = oalloc(data_00625A98[rclass] * sizeof(SInt32));
            data_005F3D70[rclass] = oalloc(data_00625A98[rclass] * sizeof(SInt32));
        }
        func_0054AD10(moveinvariants);
        freeoheap();
    }
}
