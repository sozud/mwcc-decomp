// Source-only/native code: the copied 3.0.1 back end's alias of a memory
// operand (src/psp/backend/AliasPropagation.c's func_0053BD10, b145 path).
// The rest of that unit is the 3.0.1 alias/use-def pass on MPCode's use/def
// numbers, which b69's MPCode lacks (config/exclude.txt), so only this
// function is built here (with the back end's defines: src/srconly/psp).
#include "compiler/common.h"
#include "compiler/CError.h"
#include "compiler/objects.h"
#include "psp/BE.h"

extern struct Alias* make_alias(Object* obj, SInt32 offset, SInt32 size);
extern SInt32 func_0048FBE0(UInt16 op); // bytes an opcode moves
extern SInt16 data_00625E3E; // frame register

struct Alias* func_0053BD10(VOperand* mem, MPCode* pc) {
    Object* obj;
    struct Alias* alias;

    alias = NULL;
    obj = (Object*)mem->object;
#if MWCC_CORE == MWCC_CORE_B011126
    // 011126 (0x439A80, called by every load and store helper: 0x48E1D4,
    // 0x48E346, 0x4979EC, 0x497AF0, 0x497C4B, 0x497D0B, 0x499D68): the
    // offset as it is, the object's part only when the access is based on
    // nothing or on the frame register being $sp (no datatype test)
    {
        UInt16 base = pc->args[2].reg;
        if (base != 0 && !(base == data_00625E3E && base == 29)) {
            return make_alias(obj, 0, 0);
        }
        return make_alias(obj, pc->args[1].value, func_0048FBE0(pc->op));
    }
#endif
    if (!obj) {
        CError_Internal("AliasPropagation.c", CError_B145_LINE(37));
    }
#if MWCC_CORE != MWCC_CORE_B233
    // (2.3.3 keeps the object whatever the base: 0x494A90 stores the
    // operand's object in the instruction, which is a pointer access only
    // without one)
    // b69's load and store helpers (0x48E0EA, 0x48E25D) give a local no alias
    // unless the access is based on $sp itself; gather_alias_info then gives
    // the access worst_case_ptr
    if (obj->datatype == DLOCAL && pc->args[2].reg != 29) {
        return NULL;
    }
#endif
    if (obj->datatype == DLOCAL && mem->reg != data_00625E3E) {
        alias = make_alias(obj, 0, 0);
    } else if (obj->datatype == DLOCAL) {
        alias = make_alias(
            obj,
            pc->args[1].value - ((Object*)MOP_OBJ(pc->args[1]))->u.var.offset,
            func_0048FBE0(pc->op)
        );
    } else if (obj->datatype == DDATA) {
        alias = make_alias(obj, pc->args[1].value, func_0048FBE0(pc->op));
    } else {
        CError_Internal("AliasPropagation.c", 53); // b145 has no function case
    }
    return alias;
}
