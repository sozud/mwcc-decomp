#include "compiler/mwcc_core.h"
#ifndef COMPILER_IROEXPRREGENERATION_H
#define COMPILER_IROEXPRREGENERATION_H
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void IRO_RegenerateExpressions(void);

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
