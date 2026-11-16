#include "compiler/mwcc_core.h"
#ifndef COMPILER_IRORANGEPROPAGATION_H
#define COMPILER_IRORANGEPROPAGATION_H

#include "compiler/common.h"
#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(push, 2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern Boolean IRO_RangePropagateInFNode(void);

#ifdef __cplusplus
}
#endif

#if MWCC_CORE == MWCC_CORE_B52
#pragma pack(pop)
#endif
#endif
