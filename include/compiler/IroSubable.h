#include "compiler/mwcc_core.h"
#ifndef COMPILER_IROSUBABLE_H
#define COMPILER_IROSUBABLE_H

#include "compiler/common.h"
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void IRO_InitializeIsSubableOpArray(void);
extern Boolean IRO_IsSubableExpression(IROLinear* nd);

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
