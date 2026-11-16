// b69's FEInterface.c: functions of the MIPS back end's front-end interface
// that the 3.0.1 src/psp-driver/FEInterface.c does not have. b69 (2.4.5) has
// no IRO hook table; the GameCube-era optimizer calls CodeGen_GetAsmEffects
// directly (the 3.0.1 hook is psp-driver/FEInterface.c's func_00439F30).

#include "compiler/CError.h"
#include "compiler/CFunc.h"
#include "compiler/enode.h"
#include "compiler/InlineAsm.h"
#include "compiler/InlineAsmMIPS.h"
#include "compiler/objects.h"
#include "compiler/types.h"

extern SInt32 func_0048FBE0(
    UInt16 op
); // nbytes_loaded_or_stored_by: an opcode's memory access size
extern Boolean ps2_IsQuadwordClass(Type* type);

// set after a branch: the next asm instruction is a reach barrier
Boolean asmbranchseen_b69_0057E03D;

#define REGISTER_TYPE(t) \
    ((t)->type == TYPEINT || \
     (t)->type == TYPEENUM || \
     (t)->type == TYPEPOINTER || \
     ((t)->type == TYPEMEMBERPOINTER && (t)->size == 4) || \
     (t)->type == TYPEFLOAT || \
     ps2_IsQuadwordClass(t))

// 0x4C0A40: the optimizer's copy propagation into an asm instruction: a
// variable an operand reads becomes the object it was copied from (a register
// operand only for a register-sized integer or pointer, or a float in an FP
// register; quadword classes stay)
void CodeGen_PropagateIntoAsm(Statement* stmt, Object* obj, ENode* expr) {
    MAsmInstruction* mi;
    Object* newobj;
    int i;

    mi = (MAsmInstruction*)stmt->expr;
    if (ENODE_IS(expr, EOBJREF)) {
        newobj = expr->data.objref;
        if (obj->otype == newobj->otype && obj->datatype == newobj->datatype) {
            for (i = 0; i < mi->nops; i++) {
                switch (mi->ops[i].type) {
                    case MOpnd_Reg:
                        if (mi->ops[i].u.reg.object == obj) {
                            if (newobj->type->type == TYPEINT ||
                                newobj->type->type == TYPEENUM ||
                                newobj->type->type == TYPEPOINTER ||
                                (newobj->type->type == TYPEMEMBERPOINTER &&
                                 newobj->type->size == 4)) {
                                mi->ops[i].u.reg.object = newobj;
                            } else if (newobj->type->type == TYPEFLOAT && mi->ops[i].rclass == 1) {
                                mi->ops[i].u.reg.object = newobj;
                            } else if (!ps2_IsQuadwordClass(newobj->type)) {
                                // b69 0x4C0AD9: `test al; je; jmp` to the same place
                                continue;
                            }
                        }
                        break;
                    case MOpnd_GlobalObj:
                    case MOpnd_LocalObj:
                        if (mi->ops[i].u.obj.obj == obj) {
                            mi->ops[i].u.obj.obj = newobj;
                        }
                        break;
                }
            }
        }
    }
}

// 0x4C0B00: what an inline asm instruction reads, writes and does to control
// flow, for the optimizer
void CodeGen_GetAsmEffects(Statement* stmt, IAEffects* effects) {
    MAsmInstruction* instr = (MAsmInstruction*)stmt->expr;
    OpcodeDef* def = &data_006115F8[instr->opcode];
    MAsmOperand* op;
    SInt32 i;
    MAsmOperand* b69_dead;

    effects->numoperands = 0;
    effects->numlabels = 0;
    effects->writesMemory = FALSE;
    effects->readsMemory = FALSE;
    effects->unmodeledControlFlow = FALSE;
    effects->branchWithLink = FALSE;
    effects->optimizationBarrier = FALSE;
    effects->noFallthrough = FALSE;
    effects->reachBarrier = FALSE;

    if ((UInt16)instr->flags & 4) {
        effects->reachBarrier = TRUE;
        effects->optimizationBarrier = TRUE;
    }
    if (def->flags & 0x100) {
        effects->reachBarrier = TRUE;
    }
    if (def->flags & 0x20) {
        if (def->flags & 2) {
            effects->writesMemory = TRUE;
        }
        if (def->flags & 4) {
            effects->readsMemory = TRUE;
        }
    }
    if (def->flags & 8) {
        effects->branchWithLink = TRUE;
    }
    if (instr->opcode == 0x39 ||
        instr->opcode == 0x361 ||
        instr->opcode == 0x362 ||
        instr->opcode == 0x3A ||
        instr->opcode == 0x3C ||
        instr->opcode == 0x3D ||
        instr->opcode == 0x3B) {
        effects->noFallthrough = TRUE;
        asmbranchseen_b69_0057E03D = TRUE;
    }
    if (asmbranchseen_b69_0057E03D == TRUE) {
        effects->reachBarrier = TRUE;
        asmbranchseen_b69_0057E03D = FALSE;
    }

    // b69 keeps the header's values in a dead register
    for (i = 0, b69_dead = op = instr->ops; i < instr->nops; i++, b69_dead = op++) {
        switch (op->type) {
            case MOpnd_Imm:
                break;
            case MOpnd_Reg:
                if (!op->u.reg.object) {
                    break;
                }
                if (op->effect & 1) {
                    if (REGISTER_TYPE(op->u.reg.object->type)) {
                        if (effects->numoperands &&
                            effects->operands[effects->numoperands - 1].object ==
                                op->u.reg.object &&
                            (effects->operands[effects->numoperands - 1].type == IAEffect_Write ||
                             effects->operands[effects->numoperands - 1].type ==
                                 IAEffect_ReadWrite)) {
                            effects->operands[effects->numoperands - 1].type = IAEffect_ReadWrite;
                        } else {
                            effects->operands[effects->numoperands].type = IAEffect_Read;
                            effects->operands[effects->numoperands].object = op->u.reg.object;
                            effects->operands[effects->numoperands].size =
                                op->u.reg.object->type->size;
                            effects->operands[effects->numoperands].offset = 0;
                            effects->numoperands++;
                            if (i == 2 &&
                                (instr->opcode == 0x1A9 || instr->opcode == 0x1A8) &&
                                op->u.reg.object->type->type == TYPEPOINTER) {
                                effects->writesMemory = TRUE;
                            }
                        }
                    } else {
                        effects->readsMemory = TRUE;
                    }
                }
                if (op->effect & 2) {
                    if (REGISTER_TYPE(op->u.reg.object->type)) {
                        if (effects->numoperands &&
                            effects->operands[effects->numoperands - 1].object ==
                                op->u.reg.object &&
                            (effects->operands[effects->numoperands - 1].type == IAEffect_Read ||
                             effects->operands[effects->numoperands - 1].type ==
                                 IAEffect_ReadWrite)) {
                            effects->operands[effects->numoperands - 1].type = IAEffect_ReadWrite;
                        } else {
                            effects->operands[effects->numoperands].type = IAEffect_Write;
                            effects->operands[effects->numoperands].object = op->u.reg.object;
                            effects->operands[effects->numoperands].offset = 0;
                            effects->operands[effects->numoperands].size =
                                op->u.reg.object->type->size;
                            effects->numoperands++;
                            if (i == 2 &&
                                (instr->opcode == 0x1A9 || instr->opcode == 0x1A8) &&
                                op->u.reg.object->type->type == TYPEPOINTER) {
                                effects->writesMemory = TRUE;
                            }
                        }
                    } else {
                        effects->writesMemory = TRUE;
                    }
                }
                break;
            case MOpnd_GlobalObj:
            case MOpnd_LocalObj:
                if (!op->u.obj.obj) {
                    break;
                }
                if (def->flags & 2) {
                    effects->operands[effects->numoperands].type = IAEffect_Read;
                    effects->operands[effects->numoperands].object = op->u.obj.obj;
                    effects->operands[effects->numoperands].size =
                        func_0048FBE0(ASM_BE_OP(instr->opcode));
                    effects->operands[effects->numoperands].offset = op->u.obj.offset;
                    effects->numoperands++;
                }
                if (def->flags & 4) {
                    effects->operands[effects->numoperands].type = IAEffect_Write;
                    effects->operands[effects->numoperands].object = op->u.obj.obj;
                    effects->operands[effects->numoperands].offset = op->u.obj.offset;
                    effects->operands[effects->numoperands].size =
                        func_0048FBE0(ASM_BE_OP(instr->opcode));
                    effects->numoperands++;
                }
                break;
            case MOpnd_Lab:
                effects->labels[effects->numlabels] = op->u.lab.label;
                effects->numlabels++;
                break;
            case MOpnd_GCCReg:
            case MOpnd_GCCImm:
                CError_FATAL(338);
                break;
            default:
                effects->readsMemory = TRUE;
                effects->writesMemory = TRUE;
                break;
        }
        CError_ASSERT(347, (UInt32)effects->numoperands <= IAMaxOperands);
        CError_ASSERT(350, (UInt32)effects->numlabels <= IAMaxLabels);
    }

    if ((def->flags & 9) && !effects->numlabels) {
        effects->unmodeledControlFlow = TRUE;
    }
}

// b69 0x50C320..0x50C47E (the unit of ps2_IsQuadwordClass 0x50C2B0 and
// ps2_IsQuadwordClassPointer 0x50C370; placed here for now): a class with no
// vtable whose only member is a 64-byte array (a 4x4 float matrix), a
// pointer to one, and whether an object is a pointer argument (a local in
// `arguments`) to a quadword or matrix class. GCC asm operands that are such
// arguments go through a temporary (InlineAsm_GCCLookupSymbol).
extern ObjectList* arguments;

// 0x50C2B0 (b210 0x4D2120, psp-driver/FEInterface.c, which b69 does not
// build): a class that only wraps one 128-bit integer, directly or through a
// single base class
Boolean ps2_IsQuadwordClass(Type* type) {
    TypeClass* tclass;
    ObjMemberVar* ivar;
    ClassList* base;

    if (type == NULL) {
        return FALSE;
    }
    if (IS_TYPE_CLASS(type) && TYPE_CLASS(type)->vtable == NULL) {
        tclass = TYPE_CLASS(type);
        if ((ivar = tclass->ivars) != NULL) {
            if (ivar->next != NULL) {
                return FALSE;
            }
        } else {
            if ((base = tclass->bases) != NULL) {
                if (base->base != NULL && base->next == NULL) {
                    return ps2_IsQuadwordClass(TYPE(base->base));
                }
                return FALSE;
            }
            return FALSE;
        }
        if (ivar != NULL && (ivar->type->type != TYPEINT || ivar->type->size != 16)) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

// 0x50C370 (b210 0x4D20F0): a pointer to such a class
Boolean ps2_IsQuadwordClassPointer(Type* type) {
    if (type != NULL && IS_TYPE_POINTER_ONLY(type)) { // b69: TYPEPOINTER only
        if (ps2_IsQuadwordClass(TPTR_TARGET(type))) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

// 0x50C3D0 (b210 0x4D20B0): a local variable of such a class
Boolean ps2_IsQuadwordClassLocal(Object* obj) {
    Object* o; // b69: declared first, so list->object's copy is ebx and the
    Boolean r; // dead `mov eax,ebx` (the result's register) appears
    ObjectList* list;

    if (!obj) {
        return FALSE;
    }
    if (obj->datatype != DLOCAL) {
        return FALSE;
    }
    for (list = locals; list; list = list->next) {
        o = list->object;
        if (o == obj) {
            r = ps2_IsQuadwordClass(obj->type);
            return r;
        }
    }
    return FALSE;
}

Boolean ps2_IsMatrixClass_b69_0050C320(Type* type) {
    TypeClass* tclass;
    ObjMemberVar* ivar;

    if (type == NULL) {
        return FALSE;
    }
    if (IS_TYPE_CLASS(type) && TYPE_CLASS(type)->vtable == NULL) {
        tclass = TYPE_CLASS(type);
        ivar = tclass->ivars;
        if (ivar == NULL || ivar->next != NULL) {
            return FALSE;
        }
        if (ivar != NULL && (ivar->type->type != TYPEARRAY || ivar->type->size != 64)) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

Boolean ps2_IsMatrixClassPointer_b69_0050C3A0(Type* type) {
    if (type != NULL && type->type == TYPEPOINTER) {
        if (ps2_IsMatrixClass_b69_0050C320(TPTR_TARGET(type))) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

Boolean gcc_IsQuadwordArg_b69_0050C420(Object* obj) {
    ObjectList* list;

    for (list = arguments; list; list = list->next) {
        if (list->object == obj &&
            list->object->type->type == TYPEPOINTER &&
            list->object->datatype == DLOCAL) {
            if (ps2_IsQuadwordClassPointer(obj->type)) {
                return TRUE;
            }
            if (ps2_IsMatrixClassPointer_b69_0050C3A0(obj->type)) {
                return TRUE;
            }
            return FALSE;
        }
    }
    return FALSE;
}
