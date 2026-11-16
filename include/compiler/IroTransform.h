#include "compiler/mwcc_core.h"
#ifndef COMPILER_IROTRANSFORM_H
#define COMPILER_IROTRANSFORM_H

#include "compiler/common.h"
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void IRO_InitializeAssignmentOpArray(void);
extern void IRO_InitializeComplementaryOpArray(void);
extern void IRO_InitializeComplementaryOpLogicalArray(void);
extern void IRO_DoTransformations(void);
extern Boolean IRO_TransformSelfAssignmentToAssignment(IROLinear* nd);
extern void IRO_TransformTree(Statement* statements);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#if MWCC_CORE != MWCC_CORE_B52
extern void IRO_NarrowExpressions(Statement* statements);
#endif
#endif

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
