#include "compiler/mwcc_core.h"
#ifndef COMPILER_IROUSEDEF_H
#define COMPILER_IROUSEDEF_H

#include "compiler/common.h"
#include "compiler/enode.h"
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#if MWCC_CORE == MWCC_CORE_B52
#ifdef __cplusplus
extern "C" {
#endif

// natively the pointers must stay aligned: 3.0.1's records (IROUse301) are
#pragma pack(push, 2)
struct IROUse {
#else
#ifdef __cplusplus
extern "C" {
#endif

#if MWCCPSP
// natively the pointers must stay aligned: 3.0.1's records (IROUse301) are
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#pragma pack(push, 2)
#elif !MWCCPSP_NATIVE
#pragma pack(push, 2)
#endif
struct IROUse {
#endif
    SInt32 index;
    IRONode* node;
    IROLinear* linear;
    VarRecord* var;
    IROUse* globalnext;
    IROUse* varnext;
    BitVector* reachingDefs;
    UInt16 reachingDefCount;
};
struct IRODef {
    SInt32 index;
    IRONode* node;
    IROLinear* linear;
    VarRecord* var;
    IRODef* globalnext;
    IRODef* varnext;
    UInt16 useCount;
    Boolean global;
    Boolean noregister;
    Boolean definite;
    Boolean removable;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#pragma pack(pop)
#elif !MWCCPSP_NATIVE
#pragma pack(pop)
#endif

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE == MWCC_CORE_B52
extern ENodeType IRO_NonAssignmentOp[IRO_MAXEXPR];
#else
extern ENodeType IRO_NonAssignmentOp[MAXEXPR];
#endif
#elif MWCCPSP
extern ENodeType IRO_NonAssignmentOp[IRO_MAXEXPR];
#else
extern ENodeType IRO_NonAssignmentOp[MAXEXPR];
#endif
extern IROUse* IRO_FirstVarUse;
extern IROUse* IRO_LastVarUse;

extern CInt64 IRO_GetSelfAssignmentVal(IROLinear* linear);
extern void IRO_InitializeNonAssignmentOpArray(void);
extern void IRO_InitializeAssignmentFoldingFunctionArray(void);
extern Boolean IRO_UseDef(Boolean optDeadAssignments, Boolean optPropagation);
extern void IRO_SplitLifetimes(void);

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
