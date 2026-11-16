#include "compiler/mwcc_core.h"
#ifndef COMPILER_IROEVAL_H
#define COMPILER_IROEVAL_H

#include "compiler/common.h"
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void IRO_InitializeIsAssociativeENodeTypeArray(void);
extern void IRO_TruncateValueToType(CInt64* val, Type* type);
extern void IRO_TruncateBitfieldValueToType(CInt64* val, Type* type, TypeBitfield* bitfieldType);
extern void IRO_ConstantFolding(void);
extern Boolean IRO_EvaluateConditionals(void);
extern int IRO_SimplifyConditionals(void);
extern Boolean IRO_EvaluateDefinitePointers(Object* func);

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
