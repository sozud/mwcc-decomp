#include "compiler/mwcc_core.h"
#ifndef COMPILER_INLINEASMREGISTERS_H
#define COMPILER_INLINEASMREGISTERS_H

#include "compiler/common.h"
#include "compiler/InlineAsm.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void InlineAsm_InitializeRegisters(void);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
// b69 (0x50ABA0..0x50AC60, the MIPS assembler's register names): the class is
// a short
extern void InlineAsm_InsertRegister(char* name, short rclass, short num, Object* object);
extern void* InlineAsm_LookupRegister(char* name);
#else
extern void InlineAsm_InsertRegister(char* name, RegClass rclass, short num, Object* object);
extern IARegister* InlineAsm_LookupRegister(char* name);
#endif

#ifdef __cplusplus
}
#endif

#endif
