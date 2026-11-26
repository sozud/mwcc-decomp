#ifndef DRIVER_STRINGEXTRAS_H
#define DRIVER_STRINGEXTRAS_H

#include "compiler/common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char* __stdcall strcatn(char* d, const char* s, SInt32 max);
extern char* __stdcall strcpyn(char* d, const char* s, SInt32 len, SInt32 max);
#if MWCC_CORE == MWCC_CORE_B233
#define USTRCMP_CC // 2.3.3: cdecl (ret without imm16)
#else
#define USTRCMP_CC __stdcall
#endif
extern int USTRCMP_CC ustrcmp(const char* src, const char* dst);
extern int __stdcall ustrncmp(const char* src, const char* dst, UInt32 len);

#ifdef __cplusplus
}
#endif

#endif
