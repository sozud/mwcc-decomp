#include "compiler/mwcc_core.h"
// b69's local common-subexpression elimination (LocalCSE.c, its assert file
// name; 0x4A6D20..0x4A7F55). The code generator runs it twice per function,
// after pipeline1 and after macro expansion B, with no argument. It is GC
// 2.4.7's ValueNumbering.c (value numbers per register, an available-value
// list per opcode, extended basic blocks with a stack of overwritten values)
// on the MIPS back end's pcode, before b145's store CSE: an instruction's
// result is args[1] when flag 0x80000000 is set, VU0 moves (0x1F6..0x2BE)
// have dest/src in args[1]/args[2], no instruction class is treated
// specially by isCSEop, and nothing is inlined.
//
// All 37 functions are byte-exact with Pro 6 and `-inline off`
// (config/sources.json): only that setting keeps b69's dead copies of a
// for-statement's comma expressions (`i++, op++` leaves `mov eax, esi` before
// the add) and of comma expression statements; `-inline on`/`auto` drop them.
// An assignment used as a condition re-reads memory (`cmp [mem], 0`), and
// Pro 6 lays .bss out last-declared first.
//
// b69's operands are 0x16 bytes (2-byte aligned): kind, register class,
// then either a register's flags (+2) and number (+4) or a value of up to
// 128 bits (+2: an immediate's word, a memory operand's offset with its
// object at +6), and a word at +0x12 (compared for kind 5). The rest of
// this tree's back end still uses 3.0.1's 0x18-byte MOperand, so the source-
// only and native links build this file through src/srconly/psp/
// LocalCSE_ours.c with LOCALCSE_OURS (the same code on MOperand).
#include "compiler/common.h"
#include "compiler/Alias.h"
#include "compiler/CError.h"
#include "compiler/CompilerTools.h"
#include "compiler/CInt64.h"
#include "psp/BE.h"
#if MWCC_CORE == MWCC_CORE_B233
#include <string.h>
#include "compiler/objects.h"
#include "compiler/types.h"
#include "compiler/CodeGen.h"
#endif

typedef MOperand CSEOperand;
#define OP_FLAGS(op) ((op)->flags)
#define OP_REG(op) ((op)->reg)
#define OP_WORD(op) ((op)->value)
#define OP_OBJ(op) ((op)->value2)
#define OP_CINT(op) (*(CInt64*)&(op)->value)
#define OP_LABEL(op) ((op)->ptr)
#define OP_WORD12(op) ((op)->value_mid[1])

typedef MPCode CSEPCode;
#define ARGS(pc) (((CSEPCode*)(pc))->args)

typedef struct ValueLabel {
    struct ValueLabel* next;
    CSEOperand op; // +4
} ValueLabel; // 0x1A

typedef struct AvailableValue {
    struct AvailableValue* next;
    ValueLabel* labelled; // +4
    MPCode* pcode; // +8
    int killedregister; // +0xC
    int aliasnumber; // +0x10
    int opnumbers[1]; // +0x14
} AvailableValue;

typedef struct RegValue {
    int number;
    int copyreg;
    AvailableValue* available;
} RegValue; // 0xC

typedef struct State {
    void* stackedvalues;
    int valueceiling;
} State;

typedef struct StackedValue {
    struct StackedValue* next;
    CSEOperand op; // +4
    RegValue value; // +0x1A
    Alias* alias; // +0x26
    MPCode* valuepcode; // +0x2A
} StackedValue; // 0x2E

// this tree's opcode numbering (b210's, BE.h)
#define CSE_OPCODE_MAX BE_NUM_OPDEFS
#define IS_VU0OP(op) ((op) > 0x21E && (op) <= 0x2E7)

int removedcommonsubexpressions;
int nextvaluenumber;
// Pro 6 lays .bss out last-declared first: b69 has valueceiling at 0x563600
static AvailableValue* opvalue[CSE_OPCODE_MAX];
static RegValue* regvalue[7];
static StackedValue* stackedvalues;
static int valueceiling;

extern int coloring;
extern SInt32 data_00624744[7]; // n_real_registers: the first virtual register of each class
extern SInt32 data_00625A98[7]; // used_virtual_registers
// (CParser.h only here: an extra header renumbers the hybrid object's
// literals, and matchvalues' "LocalCSE.c" is bound as LocalCSE.c/_b69at216)
#include "compiler/CParser.h"
// b69's PCodeInfo.c helpers (0x49A830, 0x49AC60, 0x49A9D0) with this tree's
// opcode numbering: the copy is func_00490280's but takes the 128-bit move
// from its third argument (b69 tests copts+0x37 and that, not the operand's
// 0x2000 flag); the store test is func_0048FFA0 (b69's has the same shape)
extern SInt16 data_00625E48; // move
extern SInt16 data_00625E74; // 16-byte register move
extern SInt32 func_0048FBE0(UInt16 op);
extern SInt32 func_0048FFA0(MPCode* store, MPCode* load);

static MPCode* mips_makecopyinstruction(CSEOperand* src, CSEOperand* dst, char wide) {
    if (src->kind == 0) {
#if MWCC_CORE == MWCC_CORE_B011126
        (void)wide; // 011126 (0x49AF50): no 128-bit copy
        if (0) {
#else
        if (copts.mips_option41 && src->rclass == 0 && wide) {
#endif
            return makepcode(data_00625E74, dst->reg, src->reg);
        }
        if (src->rclass == 0) {
            return makepcode(data_00625E48, dst->reg, src->reg);
        } else if (src->rclass == 1) {
            return makepcode(
                (src->flags & 0x8000) ? MACRO_OP(0x9A) : MACRO_OP(0x99), dst->reg, src->reg
            );
        } else if (src->rclass == 5) {
            return makepcode(OPC(0x2AF), 0xF, dst->reg, src->reg);
        } else if (src->rclass == 3) {
            return makepcode(OPC(0x281), dst->reg, src->reg, 0);
        }
    }
    CError_Internal("PCodeInfo.c", 722);
    return NULL;
}

static char mips_is128bitop(MPCode* pcode) {
#if MWCC_CORE == MWCC_CORE_B011126
    // 011126 has no mips_is128bitop (b69 0x49AC60): its callers take a plain copy
    return 0;
#endif
    if (copts.mips_option41) {
        return func_0048FBE0(pcode->op) == 16;
    }
    return 0;
}

// b69 0x49A9D0: by the store's alias size, only the quadword (copts+0x41) and
// doubleword pairs; b145's func_0048FFA0 also takes word and byte pairs
static int mips_can_reuse_stored_value(MPCode* store, MPCode* load) {
    SInt32 size;

#if MWCC_CORE == MWCC_CORE_B233
    // 2.3.3 (0x495E30): the size of the store's object's type
    {
        Object* obj = (store->argcount > 1 && ARGS(store)[1].kind == 4)
            ? (Object*)MOP_OBJ(ARGS(store)[1])
            : NULL;
        if (!obj) {
            return 0;
        }
        size = obj->type->size;
    }
#else
    if (!store->alias) {
        return 0;
    }
    size = store->alias->size;
#endif
    if (copts.mips_option41) {
        if (size > 16) {
            return 0;
        }
        if ((store->op == OPC(0x1D2) && load->op == OPC(0x1D1)) ||
            (store->op == OPC(0x489) && load->op == OPC(0x467))) {
            return 1;
        }
    }
    if (size > 8) {
        return 0;
    }
    if ((store->op == OPC(0xC4) && load->op == OPC(0x93)) ||
        (store->op == OPC(0x479) && load->op == OPC(0x454))) {
        return 1;
    }
    return 0;
}

#define RESULT_OP(pc) (&ARGS(pc)[((pc)->flags & 0x80000000) ? 1 : 0])
#define REGVALUE(op) (&regvalue[(op)->rclass][OP_REG(op)])

#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
// 2.3.3's CSE has no alias analysis: memory values are kept per object (the
// instruction's object, MPCode +0x10 in 2.3.3), in a tree of entries made at
// the start (0x4A4930: one for every object a non-pointer access names, and
// a dummy object, 0x5410C8, whose value stands for "memory through a
// pointer"). Entries are this tree's Alias records (only object, valuenumber
// and valuepcode used), so the value stacking is unchanged.
static Alias* b233_memlist;
static Alias* b233_memhash[1024];
static Object b233_pointerobj;

static Object* b233_obj(MPCode* pcode) {
    if (pcode->argcount > 1 && ARGS(pcode)[1].kind == 4) {
        return (Object*)MOP_OBJ(ARGS(pcode)[1]);
    }
    return NULL;
}

// 0x4A4870 / 0x4A48C0
static Alias* b233_mem(Object* obj) {
    unsigned h = (unsigned)(((unsigned long)obj >> 4) & 1023);
    Alias* a;

    for (a = b233_memhash[h]; a; a = a->hashNext) {
        if (a->object == obj) {
            return a;
        }
    }
    a = oalloc(sizeof(Alias));
    memset(a, 0, sizeof(Alias));
    a->object = obj;
    a->valuenumber = nextvaluenumber++;
    a->valuepcode = NULL;
    a->hashNext = b233_memhash[h];
    b233_memhash[h] = a;
    a->next = b233_memlist;
    b233_memlist = a;
    return a;
}

// the entry a load's available value is tied to (0x4A5168, 0x4A537E)
static Alias* b233_loadmem(MPCode* pcode) {
    return b233_mem((pcode->flags & 0x20) ? &b233_pointerobj : b233_obj(pcode));
}

// 0x432F50
static Boolean b233_mayalias(Object* obj) {
    VarInfo* info;
    Type* type;

    if (!obj || (obj->datatype != DDATA && obj->datatype != DLOCAL)) {
        return FALSE;
    }
    info = obj->datatype == DLOCAL ? obj->u.var.info : NULL;
#if MWCCPSP_NATIVE
    {
        // Classification aid (native only): 2.3.3 (0x432F6E..0x432F80) reads
        // +0x24 of a data object too; for a pooled float constant that is
        // its Float pointer, and the byte it tests (+0x22) lies past the
        // 8-byte Float, in whatever was allocated next (heap residue).
        // MWCCPS2_B233_FLOATRES names a file of "constant byte" lines
        // recorded from the original; a listed constant takes that byte.
        extern int b233_floatres(Object * obj);
        int b = b233_floatres(obj);
        if (b == 0) {
            goto typetest;
        }
        if (b > 0) {
            return TRUE;
        }
    }
#endif
    if (!info || info->noregister) {
        return TRUE;
    }
#if MWCCPSP_NATIVE
typetest:
#endif
    if (!(type = obj->type)) {
        return FALSE;
    }
    return type->type == TYPEARRAY ||
        type->type == TYPESTRUCT ||
        type->type == TYPECLASS ||
        (type->type == TYPEMEMBERPOINTER && type->size == 12);
}

// 0x4A5400 / 0x4A5460: an access to an aggregate object
static int b233_aggregate(MPCode* pcode) {
    Object* obj = b233_obj(pcode);
    Type* type;

    if (!obj || !(type = obj->type)) {
        return 0;
    }
    return type->type == TYPEARRAY ||
        type->type == TYPESTRUCT ||
        type->type == TYPECLASS ||
        (type->type == TYPEMEMBERPOINTER && type->size == 12);
}

void killmemory(Alias* alias, MPCode* newValue);

// 0x4A4D90: the pointer entry always; with all, every entry for an object
// that may be aliased
static void b233_killmemory(Boolean all) {
    Alias* a;

    for (a = b233_memlist; a; a = a->next) {
        if (a->object == &b233_pointerobj || (all && b233_mayalias(a->object))) {
            killmemory(a, NULL);
        }
    }
}

// 0x4A4930
static void b233_gathermemory(void) {
    MPCodeBlock* block;
    MPCode* pcode;

    b233_memlist = NULL;
    memset(b233_memhash, 0, sizeof(b233_memhash));
    memset(&b233_pointerobj, 0, sizeof(b233_pointerobj));
    b233_mem(&b233_pointerobj);
    for (block = data_00624FD0; block; block = block->next) {
        for (pcode = block->first; pcode; pcode = pcode->next) {
            if ((pcode->flags & 0x60006) && !(pcode->flags & 0x20)) {
                b233_mem(b233_obj(pcode));
            }
        }
    }
}
#endif

// 0x4A6D20
static void allocatecsedatastructures(void) {
    char rclass;

    for (rclass = 0; rclass < 7; rclass++) {
        regvalue[rclass] = oalloc(sizeof(RegValue) * data_00625A98[rclass]);
    }
}

// 0x4A6D70
static void initializecsedatastructures(void) {
    RegValue* rv;
    char rclass;
    int i;

    nextvaluenumber = 0;
    for (i = 0; i < CSE_OPCODE_MAX; i++) {
        opvalue[i] = NULL;
    }
    for (rclass = 0; rclass < 7; rclass++) {
        for (rv = regvalue[rclass], i = 0; i < data_00625A98[rclass]; i++, rv++) {
            rv->number = nextvaluenumber++;
            rv->copyreg = 0;
            rv->available = NULL;
        }
    }
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    {
        // 0x4A4AAF
        Alias* a;
        for (a = b233_memlist; a; a = a->next) {
            a->valuenumber = nextvaluenumber++;
            a->valuepcode = NULL;
        }
    }
#else
    initialize_alias_values();
#endif
    stackedvalues = NULL;
    valueceiling = 0x7FFFFFFF;
}

// 0x4A6E00
static void labelvalue(AvailableValue* av, CSEOperand* op) {
    ValueLabel* label = oalloc(sizeof(ValueLabel));
    label->op = *op;
    label->next = av->labelled, av->labelled = label;
}

// 0x4A6E30
static void unlabelvalue(AvailableValue* av, CSEOperand* op) {
    ValueLabel* labelled;
    ValueLabel** ptr;

    ptr = &av->labelled;
    while ((labelled = *ptr)) {
        if (OP_REG(&labelled->op) == OP_REG(op)) {
            *ptr = labelled->next;
        } else {
            ptr = &labelled->next;
        }
    }
}

// 0x4A6E60
static void stackregistervalue(CSEOperand* op, RegValue* value) {
    StackedValue* stacked = oalloc(sizeof(StackedValue));
    stacked->next = stackedvalues, stackedvalues = stacked;
    stacked->op = *op;
    stacked->value = *value;
}

// 0x4A6EA0
static void stackmemoryvalue(Alias* alias) {
    StackedValue* stacked = oalloc(sizeof(StackedValue));
    stacked->next = stackedvalues, stackedvalues = stacked;
    stacked->op.kind = 4;
    stacked->alias = alias;
    stacked->value.number = alias->valuenumber;
    stacked->valuepcode = (MPCode*)alias->valuepcode;
}

// 0x4A6EE0
static void unstackvalue(StackedValue* stacked) {
    CSEOperand* op = &stacked->op;
    RegValue* value;

    if (op->kind == 4) {
        stacked->alias->valuenumber = stacked->value.number;
        stacked->alias->valuepcode = (PCode*)stacked->valuepcode;
        return;
    }
    value = REGVALUE(op);
    if (value->available) {
        unlabelvalue(value->available, op);
    }
    value->number = stacked->value.number;
    value->copyreg = stacked->value.copyreg;
    if ((value->available = stacked->value.available)) {
        labelvalue(value->available, op);
    }
}

// 0x4A6F60
static int samevalue(CSEOperand* op1, CSEOperand* op2) {
    RegValue* v1 = REGVALUE(op1);
    RegValue* v2 = REGVALUE(op2);
    return v1->number == v2->number;
}

// 0x4A6FC0
static int killregister(CSEOperand* op) {
    RegValue* value = REGVALUE(op);

    if (value->number < valueceiling && nextvaluenumber >= valueceiling) {
        stackregistervalue(op, value);
    }
    if (value->available) {
        unlabelvalue(value->available, op);
    }
    value->available = NULL;
    value->copyreg = 0;
    return value->number = nextvaluenumber++;
}

// 0x4A7030 (Alias.c's update_alias_value calls it)
void killmemory(Alias* alias, MPCode* newValue) {
    if (alias->valuenumber < valueceiling && nextvaluenumber >= valueceiling) {
        stackmemoryvalue(alias);
    }
    if (newValue) {
        alias->valuenumber = REGVALUE(ARGS(newValue))->number;
        alias->valuepcode = (PCode*)newValue;
    } else {
        alias->valuenumber = nextvaluenumber++;
        alias->valuepcode = NULL;
    }
}

// 0x4A7090
static void killallCSEs(void) {
    AvailableValue* av;
    ValueLabel* labelled;
    int i;

    for (i = 0; i < CSE_OPCODE_MAX; i++) {
        for (av = opvalue[i]; av; av = av->next) {
            for (labelled = av->labelled; labelled; labelled = labelled->next) {
                killregister(&labelled->op);
            }
        }
    }
}

// 0x4A70D0
static void killregisters(MPCode* pcode) {
    CSEOperand* op;
    int i;

    for (i = 0, op = &ARGS(pcode)[0]; i < pcode->argcount; i++, op++) {
        if (op->kind == 0 && (OP_FLAGS(op) & 2)) {
            killregister(op);
        }
    }
}

// 0x4A7110
static void copyregister(CSEOperand* src, CSEOperand* dest) {
    RegValue* srcvalue = REGVALUE(src);
    RegValue* destvalue = REGVALUE(dest);

    if (destvalue->number < valueceiling && nextvaluenumber >= valueceiling) {
        stackregistervalue(dest, destvalue);
    }
    if (destvalue->available) {
        unlabelvalue(destvalue->available, dest);
    }
    if ((destvalue->available = srcvalue->available)) {
        labelvalue(destvalue->available, dest);
    }
    destvalue->number = srcvalue->number;
    if (srcvalue->copyreg && srcvalue->number == regvalue[src->rclass][srcvalue->copyreg].number) {
        destvalue->copyreg = srcvalue->copyreg;
    } else {
        destvalue->copyreg = OP_REG(src);
    }
}

// 0x4A71D0
static int matchvalues(AvailableValue* av, MPCode* match) {
    MPCode* avpc = av->pcode;
    CSEOperand* avOp = &ARGS(avpc)[0];
    CSEOperand* matchOp = &ARGS(match)[0];
    int base = (match->flags & 0x80000000) ? 1 : 0;
    int i;

#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 2.3.3 (0x4A4FA8): the instruction's object, not an alias
    if (((match->flags & 0x60006) ? b233_obj(match) : NULL) !=
            ((avpc->flags & 0x60006) ? b233_obj(avpc) : NULL) ||
        match->unk38 != avpc->unk38) {
        return FALSE;
    }
#else
    if (match->alias != avpc->alias || match->unk38 != avpc->unk38) {
        return FALSE;
    }
#endif
#if MWCC_CORE == MWCC_CORE_B233 && MWCCPS2_B233_OPS301
    // 2.3.3 keeps a relocation's object in the instruction's +0x10 (0x487AC4:
    // addiu r, $gp, sym), the word compared above; ours has it apart
    if (match->relobj != avpc->relobj) {
        return FALSE;
    }
#endif
    for (i = 0; i < match->argcount; i++, avOp++, ++matchOp) {
        if (i != base) {
            // b69's case order (code layout); each test returns on its own
            switch (avOp->kind) {
                case 0:
                    if (av->opnumbers[i] != REGVALUE(matchOp)->number) {
                        return FALSE;
                    }
                    break;
                case 4:
                    if (matchOp->kind != 4) {
                        return FALSE;
                    }
                    if (OP_OBJ(matchOp) != OP_OBJ(avOp)) {
                        return FALSE;
                    }
                    if (OP_WORD(matchOp) != OP_WORD(avOp)) {
                        return FALSE;
                    }
                    break;
                case 5:
                    if (matchOp->kind != 5) {
                        return FALSE;
                    }
                    if (OP_WORD12(matchOp) != OP_WORD12(avOp)) {
                        return FALSE;
                    }
                    if (!CInt64_Equal(OP_CINT(matchOp), OP_CINT(avOp))) {
                        return FALSE;
                    }
                    break;
                case 2:
                    if (matchOp->kind != 2) {
                        return FALSE;
                    }
                    if (OP_WORD(matchOp) != OP_WORD(avOp)) {
                        return FALSE;
                    }
                    break;
                case 3:
                    if (matchOp->kind != 3) {
                        return FALSE;
                    }
                    if (!CInt64_Equal(OP_CINT(matchOp), OP_CINT(avOp))) {
                        return FALSE;
                    }
                    break;
                case 6:
                    if (matchOp->kind != 6) {
                        return FALSE;
                    }
                    if (OP_LABEL(matchOp) != OP_LABEL(avOp)) {
                        return FALSE;
                    }
                    break;
                case 1:
                    CError_Internal("LocalCSE.c", 573);
                    break;
            }
        }
    }
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A5150: a load's value is the memory entry's (the pointer entry for
    // a pointer load)
    if ((match->flags & 0x20002) && b233_loadmem(match)->valuenumber != av->aliasnumber) {
        return FALSE;
    }
#else
    if ((match->flags & 0x20002) && match->alias->valuenumber != av->aliasnumber) {
        return FALSE;
    }
#endif
    return TRUE;
}

// 0x4A7410
static void chooselocation(AvailableValue* av, CSEOperand* op) {
    CSEOperand* baseop = RESULT_OP(av->pcode);
    ValueLabel* labelled;

    for (labelled = av->labelled; labelled; labelled = labelled->next) {
        if (OP_REG(&labelled->op) == OP_REG(baseop)) {
            *op = labelled->op;
            return;
        }
    }
    *op = av->labelled->op;
}

// 0x4A7490
static int findavailablevalue(MPCode* pcode, CSEOperand* op) {
    AvailableValue* av;
    CSEOperand swap;
    CSEOperand unswap;

    for (av = opvalue[pcode->op]; av; av = av->next) {
        if (av->labelled &&
            av->pcode->flags == pcode->flags &&
            av->pcode->argcount == pcode->argcount) {
            if (!matchvalues(av, pcode)) {
                if (!(pcode->flags & 0x2000)) {
                    continue;
                }
                swap = ARGS(pcode)[1];
                ARGS(pcode)[1] = ARGS(pcode)[2];
                ARGS(pcode)[2] = swap;
                if (!matchvalues(av, pcode)) {
                    unswap = ARGS(pcode)[1];
                    ARGS(pcode)[1] = ARGS(pcode)[2];
                    ARGS(pcode)[2] = unswap;
                    continue;
                }
            }
            chooselocation(av, op);
            return TRUE;
        }
    }
    return FALSE;
}

// 0x4A7590
static void addavailablevalue(MPCode* pcode) {
    AvailableValue* av;
    CSEOperand* op;
    int i;

    av = oalloc(sizeof(AvailableValue) - sizeof(int) + sizeof(int) * pcode->argcount);
    av->labelled = NULL;
    av->pcode = pcode;
    for (i = 0, op = &ARGS(pcode)[0]; i < pcode->argcount; i++, op++) {
        if (op->kind == 0) {
            av->opnumbers[i] = REGVALUE(op)->number;
        }
    }
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A536A: loads only
    av->aliasnumber = 0;
    if (pcode->flags & 2) {
        av->aliasnumber = b233_loadmem(pcode)->valuenumber;
    }
#else
    if (pcode->flags & 0x20002) {
        av->aliasnumber = pcode->alias->valuenumber;
    }
#endif
    op = RESULT_OP(pcode);
    av->killedregister = killregister(op);
    labelvalue(av, op);
    REGVALUE(op)->available = av;
    av->next = opvalue[pcode->op], opvalue[pcode->op] = av;
}

// 0x4A7680: no other operand is the result register
static int isCSEop(MPCode* pcode) {
    CSEOperand* baseOp = RESULT_OP(pcode);
    CSEOperand* op;
    int i;

    for (i = 0, op = &ARGS(pcode)[0]; i < pcode->argcount; i++, op++) {
        if (op != baseOp &&
            op->kind == baseOp->kind &&
            op->rclass == baseOp->rclass &&
            OP_REG(op) == OP_REG(baseOp)) {
            return FALSE;
        }
    }
    return TRUE;
}

// 0x4A7710: exactly one register is written
static int isCSEload(MPCode* pcode) {
    CSEOperand* op;
    int i;
    int count;

    count = 0;
    for (i = 0, op = &ARGS(pcode)[0]; i < pcode->argcount; i++, op++) {
        if (op->kind == 0 && (OP_FLAGS(op) & 2)) {
            count++;
        }
    }
    return count == 1;
}

// 0x4A7750
#if MWCC_CORE == MWCC_CORE_B52
static Boolean cse_doit = TRUE;
#define CSE_DOIT cse_doit&&
#define CSE_NOTLOAD 0x200080
#else
#define CSE_DOIT
#define CSE_NOTLOAD 0x80
#endif

static void registercopy(MPCode* pcode) {
    CSEOperand* dest;
    CSEOperand* src;

    dest = IS_VU0OP(pcode->op) ? &ARGS(pcode)[1] : &ARGS(pcode)[0];
    src = IS_VU0OP(pcode->op) ? &ARGS(pcode)[2] : &ARGS(pcode)[1];
    if (CSE_DOIT samevalue(src, dest)) {
        deletepcode(pcode);
    } else {
        copyregister(src, dest);
    }
}

// 0x4A77C0
static MPCode* recentlystored(Alias* alias, CSEOperand* op) {
    MPCode* pc;

    if ((pc = (MPCode*)alias->valuepcode) && alias->valuenumber == REGVALUE(&ARGS(pc)[0])->number) {
        *op = ARGS(pc)[0];
        return pc;
    } else {
        return NULL;
    }
}

// 0x4A7810
static void simpleload(MPCode* pcode) {
    CSEOperand* origOp;
    CSEOperand op;
    MPCode* rs;

    origOp = RESULT_OP(pcode);
    if ((pcode->flags & CSE_NOTLOAD) || !isCSEload(pcode)) {
        killregisters(pcode);
    } else if (CSE_DOIT findavailablevalue(pcode, &op)) {
        if (!samevalue(origOp, &op)) {
            insertpcodebefore(pcode, mips_makecopyinstruction(&op, origOp, mips_is128bitop(pcode)));
            copyregister(&op, origOp);
        }
        deletepcode(pcode);
        removedcommonsubexpressions = TRUE;
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
        // 2.3.3 (0x4A5400 -> 0x4A5770): a load from an aggregate (struct,
        // class, array, 12-byte member pointer) takes no stored value;
        // otherwise the object's entry (0x4A55C0)
    } else if (!b233_aggregate(pcode) &&
               (rs = recentlystored(b233_mem(b233_obj(pcode)), &op)) &&
               mips_can_reuse_stored_value(rs, pcode)) {
#else
    } else if (CSE_DOIT(rs = recentlystored(pcode->alias, &op)) &&
               mips_can_reuse_stored_value(rs, pcode)) {
#endif
        if (!samevalue(origOp, &op)) {
            insertpcodebefore(pcode, mips_makecopyinstruction(&op, origOp, mips_is128bitop(pcode)));
            copyregister(&op, origOp);
        }
        deletepcode(pcode);
        removedcommonsubexpressions = TRUE;
    } else {
        addavailablevalue(pcode);
    }
}

// 0x4A7940
static void simplestore(MPCode* pcode) {
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A5730 (0x4A5820 for an aggregate: no stored value kept): a store to
    // an object that may be aliased ends what pointer loads hold
    Object* obj = b233_obj(pcode);
    if (b233_mayalias(obj)) {
        b233_killmemory(FALSE);
    }
    killmemory(b233_mem(obj), b233_aggregate(pcode) ? NULL : pcode);
    killregisters(pcode);
#else
    update_alias_value(pcode->alias, (PCode*)pcode);
    killregisters(pcode);
#endif
}

// 0x4A7960
static void pointerload(MPCode* pcode) {
    CSEOperand* op;
    CSEOperand buf;

    op = RESULT_OP(pcode);
    if ((pcode->flags & CSE_NOTLOAD) || !isCSEload(pcode)) {
        killregisters(pcode);
    } else if (CSE_DOIT findavailablevalue(pcode, &buf)) {
        if (!samevalue(op, &buf)) {
            insertpcodebefore(pcode, mips_makecopyinstruction(&buf, op, mips_is128bitop(pcode)));
            copyregister(&buf, op);
        }
        deletepcode(pcode);
        removedcommonsubexpressions = TRUE;
    } else {
        addavailablevalue(pcode);
    }
}

// 0x4A7A20
static void pointerstore(MPCode* pcode) {
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A5910
    b233_killmemory(TRUE);
    killregisters(pcode);
#else
    update_alias_value(pcode->alias, NULL);
    killregisters(pcode);
#endif
}

// 0x4A7A40
static void arithmeticop(MPCode* pcode) {
    CSEOperand* op;
    CSEOperand buf;

    op = RESULT_OP(pcode);
    if (CSE_DOIT findavailablevalue(pcode, &buf)) {
        if (!samevalue(op, &buf)) {
            insertpcodebefore(pcode, mips_makecopyinstruction(&buf, op, mips_is128bitop(pcode)));
            copyregister(&buf, op);
        }
        deletepcode(pcode);
        removedcommonsubexpressions = TRUE;
    } else {
        addavailablevalue(pcode);
    }
}

// 0x4A7AF0
static void functioncall(MPCode* pcode) {
    killregisters(pcode);
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A59C0: only memory that may be aliased; after coloring every
    // available value and stored value goes
    b233_killmemory(TRUE);
    if (coloring) {
        killallCSEs();
    }
    if (coloring) {
        Alias* a;
        for (a = b233_memlist; a; a = a->next) {
            a->valuepcode = NULL;
        }
    }
    return;
#endif
    if (coloring) {
        update_all_alias_values();
        killallCSEs();
    } else {
        update_all_alias_values();
    }
}

// 0x4A7B20
static void operatefrommemory(MPCode* pcode) {
    CSEOperand* op;
    CSEOperand buf;

    op = RESULT_OP(pcode);
    if (!(pcode->flags & 0x4000) || !isCSEop(pcode) || (pcode->flags & 0x80)) {
        killregisters(pcode);
    } else if (CSE_DOIT findavailablevalue(pcode, &buf)) {
        if (!samevalue(op, &buf)) {
            insertpcodebefore(pcode, mips_makecopyinstruction(&buf, op, mips_is128bitop(pcode)));
            copyregister(&buf, op);
        }
        deletepcode(pcode);
        removedcommonsubexpressions = TRUE;
    } else {
        addavailablevalue(pcode);
    }
}

// 0x4A7BF0
static void operatetomemory(MPCode* pcode) {
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    // 0x4A5AE0
    if (pcode->flags & 0x20) {
        b233_killmemory(TRUE);
    } else {
        Object* obj = b233_obj(pcode);
        if (b233_mayalias(obj)) {
            b233_killmemory(FALSE);
        }
        killmemory(b233_mem(obj), NULL);
    }
    killregisters(pcode);
#else
    update_alias_value(pcode->alias, NULL);
    killregisters(pcode);
#endif
}

// 0x4A7C10
static void propagatecopiesto(MPCode* pcode) {
    CSEOperand* op;
    RegValue* value;
    int i;

    for (i = 0, op = &ARGS(pcode)[0]; i < pcode->argcount; i++, op++) {
        if (op->kind == 0 && (OP_FLAGS(op) & 0xB) == 1 && OP_REG(op) >= data_00624744[op->rclass]) {
            value = REGVALUE(op);
            if (value->copyreg &&
                value->copyreg >= data_00624744[op->rclass] &&
                value->number == regvalue[op->rclass][value->copyreg].number) {
                OP_REG(op) = value->copyreg;
            }
        }
    }
}

// 0x4A7C90
static void removecsesfrombasicblock(MPCodeBlock* block) {
    MPCode* pcode;
    MPCode* next;

#if MWCC_CORE == MWCC_CORE_B52
    // b52 (0x4BDAB0): an asm/noreorder (0x180) block only records values
    cse_doit = !(block->flags2E & 0x180) && block->pcodecount;
#endif
    for (pcode = block->first; pcode; pcode = next) {
        next = pcode->next;
#if MWCC_CORE == MWCC_CORE_B52
        if (cse_doit)
#endif
            propagatecopiesto(pcode);
        if (pcode->flags & 0x10) {
            registercopy(pcode);
        } else if (pcode->flags & 8) {
            functioncall(pcode);
        } else if (pcode->flags & 2) {
            if (pcode->flags & 0x20) {
                pointerload(pcode);
            } else {
                simpleload(pcode);
            }
        } else if (pcode->flags & 4) {
            if (pcode->flags & 0x20) {
                pointerstore(pcode);
            } else {
                simplestore(pcode);
            }
        } else if (pcode->flags & 0x20000) {
            operatefrommemory(pcode);
        } else if (pcode->flags & 0x40000) {
            operatetomemory(pcode);
        } else if ((pcode->flags & 0x4000) && isCSEop(pcode)) {
            arithmeticop(pcode);
        } else {
            killregisters(pcode);
        }
    }
    block->flags2E |= 4;
}

// 0x4A7D70
static void getvaluestate(State* state) {
    state->stackedvalues = stackedvalues;
    state->valueceiling = valueceiling;
}

// 0x4A7D90
static void setvaluestate(State* state) {
    stackedvalues = state->stackedvalues;
    valueceiling = state->valueceiling;
}

// 0x4A7DB0
static void forkvaluestate(int number) {
    stackedvalues = NULL;
    valueceiling = number;
}

// 0x4A7DD0
static void regressvaluestate(void) {
    AvailableValue* av;
    AvailableValue** ptr;
    int i;
    StackedValue* stacked;

    for (i = 0; i < CSE_OPCODE_MAX; i++) {
        ptr = &opvalue[i];
        while ((av = *ptr)) {
            if (av->killedregister >= valueceiling) {
                *ptr = av->next;
            } else {
                ptr = &av->next;
            }
        }
    }
    for (stacked = stackedvalues; stacked; stacked = stacked->next) {
        unstackvalue(stacked);
    }
}

// 0x4A7E20
static void removecsesfromextendedbasicblock(MPCodeBlock* block) {
    MPBlockLink* succ;
    MPCodeBlock* succblock;
    int counter;
    State state;

    removecsesfrombasicblock(block);
    while (block->successors &&
           !block->successors->next &&
           block->successors->block->predecessors &&
           !block->successors->block->predecessors->nextpred) {
        removecsesfrombasicblock(block = block->successors->block);
    }
    counter = 0;
    for (succ = block->successors; succ && (succblock = succ->block, 1); succ = succ->next) {
        if (!(succblock->flags2E & 4) &&
            succblock->predecessors &&
            !succblock->predecessors->nextpred) {
            counter++;
        }
    }
    if (!counter) {
        return;
    }
    getvaluestate(&state);
    forkvaluestate(nextvaluenumber);
    for (succ = block->successors; succ && (succblock = succ->block, 1); succ = succ->next) {
        if (!(succblock->flags2E & 4) &&
            succblock->predecessors &&
            !succblock->predecessors->nextpred) {
            removecsesfromextendedbasicblock(succblock);
            regressvaluestate();
        }
    }
    setvaluestate(&state);
}

// 0x4A7F00
void BE_RemoveCommonSubexpressions(void) {
    MPCodeBlock* block;

    removedcommonsubexpressions = FALSE;
    gather_alias_info();
#if MWCC_CORE == MWCC_CORE_B233 && LOCALCSE_OURS
    allocatecsedatastructures();
    b233_gathermemory();
#else
    allocatecsedatastructures();
#endif
    for (block = data_00624FD0; block; block = block->next) {
        block->flags2E &= ~4;
    }
    for (block = data_00624FD0; block; block = block->next) {
        if (!(block->flags2E & 4)) {
            initializecsedatastructures();
            removecsesfromextendedbasicblock(block);
        }
    }
    freeoheap();
}
