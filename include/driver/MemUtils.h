#ifndef DRIVER_MEMUTILS_H
#define DRIVER_MEMUTILS_H

#include "compiler/mwcc_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MWCC_CORE == MWCC_CORE_B233
// 2.3.3: cdecl (its callers pop the arguments: Path_Init 0x4280FA, ret without imm16)
#define MEMUTILS_CC
#else
#define MEMUTILS_CC __stdcall
#endif

extern void* MEMUTILS_CC xmalloc(const char* what, int size);
extern void* MEMUTILS_CC xcalloc(const char* what, int size);
extern void* MEMUTILS_CC xrealloc(const char* what, void* old, int size);
extern char* MEMUTILS_CC xstrdup(const char* str);
extern void MEMUTILS_CC xfree(void* ptr);

#ifdef __cplusplus
}
#endif

#endif
