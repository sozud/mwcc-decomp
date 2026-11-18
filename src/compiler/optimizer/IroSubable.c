#include "compiler/mwcc_core.h"
#include "compiler/IroSubable.h"
#if MWCCPSP
#include "compiler/IroVars.h"
#endif
#include "compiler/IroLinearForm.h"
#include "compiler/IroPropagate.h"
#include "compiler/IroUtil.h"
#include "compiler/enode.h"
#include "compiler/objects.h"
#include "compiler/types.h"
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
#include "compiler/CParser.h"
#endif
#endif

#if MWCCPSP // 3.0.1's IRO_ExprsSameSemantically (IroSimplifyIfThenElse.c) reads it
Boolean IsSubableOp[IRO_MAXEXPR];
#else
static Boolean IsSubableOp[MAXEXPR];
#endif

#if MWCCPSP // 3.0.1 entries, generated from the original's stores
void IRO_InitializeIsSubableOpArray(void) {
    int i;

    for (i = 0; i < IRO_MAXEXPR; i++) {
        IsSubableOp[i] = FALSE;
    }

    IsSubableOp[EPOSTINC] = FALSE;
    IsSubableOp[EPOSTDEC] = FALSE;
    IsSubableOp[EPREINC] = FALSE;
    IsSubableOp[EPREDEC] = FALSE;
    IsSubableOp[EINDIRECT] = FALSE;
    IsSubableOp[EMONMIN] = FALSE;
    IsSubableOp[EBINNOT] = FALSE;
    IsSubableOp[ELOGNOT] = FALSE;
    IsSubableOp[EFORCELOAD] = FALSE;
    IsSubableOp[EMUL] = TRUE;
    IsSubableOp[EMULV] = TRUE;
    IsSubableOp[EDIV] = FALSE;
    IsSubableOp[EMODULO] = FALSE;
    IsSubableOp[EADDV] = TRUE;
    IsSubableOp[ESUBV] = FALSE;
    IsSubableOp[EADD] = TRUE;
    IsSubableOp[ESUB] = FALSE;
    IsSubableOp[ESHL] = FALSE;
    IsSubableOp[ESHR] = FALSE;
    IsSubableOp[ELESS] = FALSE;
    IsSubableOp[EGREATER] = FALSE;
    IsSubableOp[ELESSEQU] = FALSE;
    IsSubableOp[EGREATEREQU] = FALSE;
    IsSubableOp[EEQU] = TRUE;
    IsSubableOp[ENOTEQU] = TRUE;
    IsSubableOp[EAND] = TRUE;
    IsSubableOp[EXOR] = TRUE;
    IsSubableOp[EOR] = TRUE;
    IsSubableOp[ELAND] = FALSE;
    IsSubableOp[ELOR] = FALSE;
    IsSubableOp[EASS] = FALSE;
    IsSubableOp[EMULASS] = FALSE;
    IsSubableOp[EDIVASS] = FALSE;
    IsSubableOp[EMODASS] = FALSE;
    IsSubableOp[EADDASS] = FALSE;
    IsSubableOp[ESUBASS] = FALSE;
    IsSubableOp[ESHLASS] = FALSE;
    IsSubableOp[ESHRASS] = FALSE;
    IsSubableOp[EANDASS] = FALSE;
    IsSubableOp[EXORASS] = FALSE;
    IsSubableOp[EORASS] = FALSE;
    IsSubableOp[ECOMMA] = FALSE;
    IsSubableOp[EMIN] = TRUE;
    IsSubableOp[EMAX] = TRUE;
    IsSubableOp[EPMODULO] = FALSE;
    IsSubableOp[EROTL] = FALSE;
    IsSubableOp[EROTR] = FALSE;
    IsSubableOp[EBCLR] = FALSE;
    IsSubableOp[EBTST] = FALSE;
    IsSubableOp[EBSET] = FALSE;
    IsSubableOp[ETYPCON] = FALSE;
    IsSubableOp[EBITFIELD] = FALSE;
    IsSubableOp[EINTCONST] = FALSE;
    IsSubableOp[EFLOATCONST] = FALSE;
    IsSubableOp[ESTRINGCONST] = FALSE;
    IsSubableOp[ECOND] = FALSE;
    IsSubableOp[EFUNCCALL] = FALSE;
    IsSubableOp[EFUNCCALLP] = FALSE;
    IsSubableOp[EOBJREF] = FALSE;
    IsSubableOp[ENULLCHECK] = FALSE;
    IsSubableOp[EPRECOMP] = FALSE;
    IsSubableOp[ELABEL] = FALSE;
    IsSubableOp[ESCOPEBEGIN] = FALSE;
    IsSubableOp[ESCOPEEND] = FALSE;
    IsSubableOp[EMFPOINTER] = FALSE;
    IsSubableOp[ETEMP] = FALSE;
    IsSubableOp[ELOCOBJ] = FALSE;
    IsSubableOp[EARGOBJ] = FALSE;
    IsSubableOp[ESETCONST] = FALSE;
    IsSubableOp[ENEWEXCEPTION] = FALSE;
    IsSubableOp[ENEWEXCEPTIONARRAY] = FALSE;
    IsSubableOp[EINITTRYCATCH] = FALSE;
    IsSubableOp[EOBJACCESS] = FALSE;
    IsSubableOp[ETEMPLDEP] = FALSE;
    IsSubableOp[EPOINTERSTAR] = FALSE;
    IsSubableOp[EDOTSTAR] = FALSE;
    IsSubableOp[ECTORINIT] = FALSE;
    IsSubableOp[ESTMT] = FALSE;
    IsSubableOp[EINSTRUCTION] = FALSE;
    IsSubableOp[EDEFINE] = FALSE;
    IsSubableOp[EREUSE] = FALSE;
    IsSubableOp[EASSBLK] = FALSE;
    IsSubableOp[ECONDASS] = FALSE;
}
#else
void IRO_InitializeIsSubableOpArray(void) {
    int i;

    for (i = 0; i < MAXEXPR; i++) {
        IsSubableOp[i] = FALSE;
    }

    IsSubableOp[EPOSTINC] = FALSE;
    IsSubableOp[EPOSTDEC] = FALSE;
    IsSubableOp[EPREINC] = FALSE;
    IsSubableOp[EPREDEC] = FALSE;
    IsSubableOp[EINDIRECT] = FALSE;
    IsSubableOp[EMONMIN] = TRUE;
    IsSubableOp[EBINNOT] = TRUE;
    IsSubableOp[ELOGNOT] = TRUE;
    IsSubableOp[EFORCELOAD] = FALSE;
    IsSubableOp[EMUL] = TRUE;
    IsSubableOp[EMULV] = TRUE;
    IsSubableOp[EDIV] = TRUE;
    IsSubableOp[EMODULO] = TRUE;
    IsSubableOp[EADDV] = TRUE;
    IsSubableOp[ESUBV] = TRUE;
    IsSubableOp[EADD] = TRUE;
    IsSubableOp[ESUB] = TRUE;
    IsSubableOp[ESHL] = TRUE;
    IsSubableOp[ESHR] = TRUE;
    IsSubableOp[ELESS] = FALSE;
    IsSubableOp[EGREATER] = FALSE;
    IsSubableOp[ELESSEQU] = FALSE;
    IsSubableOp[EGREATEREQU] = FALSE;
    IsSubableOp[EEQU] = FALSE;
    IsSubableOp[ENOTEQU] = FALSE;
    IsSubableOp[EAND] = TRUE;
    IsSubableOp[EXOR] = TRUE;
    IsSubableOp[EOR] = TRUE;
    IsSubableOp[ELAND] = FALSE;
    IsSubableOp[ELOR] = FALSE;
    IsSubableOp[EASS] = FALSE;
    IsSubableOp[EMULASS] = FALSE;
    IsSubableOp[EDIVASS] = FALSE;
    IsSubableOp[EMODASS] = FALSE;
    IsSubableOp[EADDASS] = FALSE;
    IsSubableOp[ESUBASS] = FALSE;
    IsSubableOp[ESHLASS] = FALSE;
    IsSubableOp[ESHRASS] = FALSE;
    IsSubableOp[EANDASS] = FALSE;
    IsSubableOp[EXORASS] = FALSE;
    IsSubableOp[EORASS] = FALSE;
    IsSubableOp[ECOMMA] = FALSE;
    IsSubableOp[EPMODULO] = FALSE;
    IsSubableOp[EROTL] = FALSE;
    IsSubableOp[EROTR] = FALSE;
    IsSubableOp[EBCLR] = FALSE;
    IsSubableOp[EBTST] = FALSE;
    IsSubableOp[EBSET] = FALSE;
    IsSubableOp[ETYPCON] = FALSE;
    IsSubableOp[EBITFIELD] = FALSE;
    IsSubableOp[EINTCONST] = FALSE;
    IsSubableOp[EFLOATCONST] = FALSE;
    IsSubableOp[ESTRINGCONST] = FALSE;
    IsSubableOp[ECOND] = FALSE;
    IsSubableOp[EFUNCCALL] = FALSE;
    IsSubableOp[EFUNCCALLP] = FALSE;
    IsSubableOp[EOBJREF] = FALSE;
    IsSubableOp[EMFPOINTER] = FALSE;
    IsSubableOp[ENULLCHECK] = FALSE;
    IsSubableOp[EPRECOMP] = FALSE;
    IsSubableOp[ETEMP] = FALSE;
    IsSubableOp[EARGOBJ] = FALSE;
    IsSubableOp[ELOCOBJ] = FALSE;
    IsSubableOp[ELABEL] = FALSE;
    IsSubableOp[ESETCONST] = FALSE;
    IsSubableOp[ENEWEXCEPTION] = FALSE;
    IsSubableOp[ENEWEXCEPTIONARRAY] = FALSE;
    IsSubableOp[EOBJLIST] = FALSE;
    IsSubableOp[EMEMBER] = FALSE;
    IsSubableOp[ETEMPLDEP] = FALSE;
    IsSubableOp[EINSTRUCTION] = FALSE;
    IsSubableOp[EDEFINE] = FALSE;
    IsSubableOp[EREUSE] = FALSE;
    IsSubableOp[EASSBLK] = FALSE;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
    IsSubableOp[EVECTOR128CONST] = FALSE;
#endif
    IsSubableOp[ECONDASS] = FALSE;
}
#endif

static int IsSubscript(IROLinear* nd) {
    return FALSE;
}

#if MWCC_CORE == MWCC_CORE_B52
#include "compiler/IrOptimizer.h"
#include "compiler/CParser.h"
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
extern Boolean func_004323D0(CInt64 value); // CodeGen.c: needs more than one instruction

#if MWCC_CORE == MWCC_CORE_B52
extern Boolean IRO_IsArithmeticOp[]; // IrOptimizer.c
#endif
// b69 0x4E1B00: b210 moved the operand case into a back-end hook (IRO_Hook0,
// FEInterface.c's func_004C3EC0); b69 has it here. A constant that takes more
// than one instruction to load, a float constant, and (#pragma
// irocseglobaladdresses) a global's address are common subexpressions; an
// address plus constant of a global is not.
#endif
Boolean IRO_IsSubableExpression(IROLinear* nd) {
    Object* varobj;
#if MWCC_CORE == MWCC_CORE_B52
    IROLinear* left;
    Object* obj;
    Type* rtype;
#else
    Boolean result;
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
    Boolean ok;
#endif
#endif

    switch (nd->type) {
        case IROLinearOp2Arg:
#if MWCC_CORE == MWCC_CORE_B52
            if ((nd->nodetype == EADD || nd->nodetype == ESUB) &&
                IRO_IsConstant(nd->u.diadic.right)) {
                varobj = IRO_IsVariable(nd->u.diadic.left);
                if (varobj && varobj->datatype == DLOCAL) {
#else
            if (nd->nodetype == EADD || nd->nodetype == ESUB) {
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
                    if (IRO_IsConstant(nd->u.diadic.right)) {
                        if ((varobj = IRO_IsVariable(nd->u.diadic.left)) &&
                            varobj->datatype == DLOCAL) {
#endif
                            if (varobj->u.var.info && !varobj->u.var.info->noregister) {
#else
                if (IRO_IsConstant(nd->u.diadic.right) &&
                    (varobj = IRO_IsVariable(nd->u.diadic.left)) &&
                    varobj->datatype == DLOCAL &&
                    varobj->u.var.info &&
                    !varobj->u.var.info->noregister) {
#endif
                                return FALSE;
                            }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE == MWCC_CORE_B52
                        } else {
                            left = nd->u.diadic.left;
                            if (left->type == IROLinearOperand &&
                                left->u.node->type == EOBJREF &&
                                (obj = left->u.node->data.objref) &&
                                obj->datatype != DLOCAL) {
#else
                } else if (nd->u.diadic.left->type == IROLinearOperand &&
                           nd->u.diadic.left->u.node->type == EOBJREF &&
                           (varobj = nd->u.diadic.left->u.node->data.objref) &&
                           varobj->datatype != DLOCAL) {
#endif
                                return FALSE;
#endif
                            }
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
                        }
                    }
#if MWCC_CORE == MWCC_CORE_B52
                    return IRO_IsArithmeticOp[nd->nodetype] && !IsSubscript(nd);
#else
    result = FALSE;
    if (IsSubableOp[nd->nodetype]) {
        ok = FALSE;
        if (!IsSubscript(nd)) {
            ok = TRUE;
        }
        if (ok) {
            result = TRUE;
        }
    }
#endif
#else
            result = IsSubableOp[nd->nodetype] && !IsSubscript(nd);
#endif
#if MWCC_CORE != MWCC_CORE_B52
                    return result;
#endif
                    case IROLinearOp1Arg:
#if MWCC_CORE == MWCC_CORE_B52
                        if (IRO_IsArithmeticOp[nd->nodetype] && !IsSubscript(nd)) {
#else
            if (IsSubableOp[nd->nodetype] && !IsSubscript(nd)) {
#endif
                            return TRUE;
                        }

                        if (nd->nodetype == EINDIRECT && !(nd->flags & IROLF_Assigned)) {
                            if (nd->flags & IROLF_Ind) {
                                nd = nd->u.monadic;
                                if (nd->type == IROLinearOperand &&
                                    nd->u.node->type == EOBJREF &&
                                    nd->u.node->data.objref->datatype == DLOCAL &&
                                    nd->u.node->data.objref->u.var.info &&
                                    !nd->u.node->data.objref->u.var.info->noregister) {
                                    return FALSE;
                                }
                                return TRUE;
                            }

                            if (IRO_IsVariable(nd) &&
                                IRO_IsRegable(nd->u.monadic->u.node->data.objref)) {
                                return FALSE;
                            }
                            return TRUE;
#if MWCC_CORE == MWCC_CORE_B52
                        }
                        if (nd->nodetype == ETYPCON) {
                            rtype = nd->rtype;
                            if (IS_TYPE_INT(rtype) &&
                                TYPE_INTEGRAL(rtype)->integral < 0x11 &&
                                rtype->size >= nd->u.monadic->rtype->size) {
#else
            } else if (nd->nodetype == ETYPCON &&
                       IS_TYPE_INT(nd->rtype) &&
                       nd->rtype->size >= nd->u.monadic->rtype->size) {
#endif
                                return TRUE;
#if MWCC_CORE != MWCC_CORE_B52
                            } else {
                                return FALSE;
#endif
                            }
#if MWCC_CORE == MWCC_CORE_B52
                        }
                        return FALSE;
#endif
                    case IROLinearOperand:
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE == MWCC_CORE_B52
                        if (nd->u.node &&
                            nd->u.node->type == EOBJREF &&
                            IRO_Hook12 &&
                            ((Boolean (*)(ENode*))IRO_Hook12)(nd->u.node)) {
#else
        if (copts.irocseglobaladdresses &&
            nd->u.node &&
            nd->u.node->type == EOBJREF &&
            (varobj = nd->u.node->data.objref) &&
            varobj->type->type != TYPEFUNC &&
            varobj->datatype != DLOCAL &&
            !is_volatile_object(varobj)) {
#endif
                            return TRUE;
                        }
#if MWCC_CORE == MWCC_CORE_B52
                        if (copts.opt_pulloutconstants) {
#endif
                            if (IRO_IsIntConstant(nd) && func_004323D0(nd->u.node->data.intval)) {
                                return TRUE;
                            }
                            if (IRO_IsFloatConstant(nd)) {
                                return TRUE;
#if MWCC_CORE == MWCC_CORE_B52
                            }
#endif
                        }
                        return FALSE;
#endif
                    default:
                        return FALSE;
                }
            }

            Boolean IRO_IsVectorTempCandidate(IROLinear * nd) {
                return (nd->type == IROLinearOp1Arg ||
                        nd->type == IROLinearOp2Arg ||
                        nd->type == IROLinearOp3Arg ||
                        nd->type == IROLinearOperand ||
                        nd->type == IROLinearFunccall) &&
                    ((nd->flags & IROLF_LoopInvariant) && !(nd->flags & IROLF_Ind));
            }
