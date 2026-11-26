#include "compiler/mwcc_core.h"
// mwccpsp 3.0.1 MIPSError.c (0x4C5940): the MIPS back end's diagnostics, as
// 2.4.7's PPCError.c
#include "compiler/common.h"
#include "compiler/CError.h"
#include "compiler/CParser.h"
#include "compiler/InlineAsm.h"
#include "driver/COSToolsCLT.h"
#include <setjmp.h>
#include <stdarg.h>

static void MIPSError_GetErrorString(char* str, short code) {
#if MWCC_CORE == MWCC_CORE_B011126
    // 011126 (0x4BAB40): its codes are 100..109, the resource's strings from 1 (the
    // codes below 100 some shared call sites pass are b210's and stay as they were)
    if (code >= 100 && code < 110) {
        COS_GetString(str, 10001, code - 99);
        return;
    }
#endif
#if MWCCPS2 && MWCCPS2_BUILD == 119
    CError_ASSERT(45, code >= 1 && code < 32); // b119 has 31 messages
#else
    CError_ASSERT(45, code >= 1 && code < 35);
#endif
    COS_GetString(str, 10001, code + 500);
}

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
static void MIPSError_VAErrorMessage(
    int code, va_list list, Boolean isTerminal, Boolean isWarning
) {
#else
static void MIPSError_VAErrorMessage(
    int code, va_list list, Boolean isTerminal, Boolean isWarning
) {
#endif
    char format[256];

    MIPSError_GetErrorString(format, code);
#if MWCC_CORE == MWCC_CORE_B011126
    if (code >= 100 && code < 110) {
        CError_ErrorMessageVA(code + 10001, format, list, isTerminal, isWarning); // 0x4BABBA
        return;
    }
#endif
    CError_ErrorMessageVA(code + 36000, format, list, isTerminal, isWarning);
}

#if MWCCPS2
// 0x4C5890 (b210 0x4D2190): MIPSError_Warning, quiet inside a try or under
// #pragma suppress_warnings
void func_004C5890(int code, ...) {
    va_list list;

#if MWCC_CORE == MWCC_CORE_B233
    if (trychain || COPTS_T.supress_warnings) {
#else
    if (trychain || copts.supress_warnings) {
#endif
        return;
    }

    va_start(list, code);
    MIPSError_VAErrorMessage(code, list, FALSE, TRUE);
    va_end(list);
}
#endif

// 0x4C5940: MIPSError_Error
void func_004C5940(int code, ...) {
    va_list list;

    if (trychain) {
        longjmp(trychain->jmpbuf, 1);
    }

    va_start(list, code);
    MIPSError_VAErrorMessage(code, list, FALSE, FALSE);
    va_end(list);

    if (in_assembler) {
        AssemblerError();
    }
}

#if !MWCCPS2 // b210: above, at its position
// 0x4C5890: MIPSError_Warning
void func_004C5890(int code, ...) {
    va_list list;

#if MWCC_CORE == MWCC_CORE_B233
    if (trychain || COPTS_T.supress_warnings) {
#else
    if (trychain || copts.supress_warnings) {
#endif
        return;
    }
    va_start(list, code);
    MIPSError_VAErrorMessage(code, list, FALSE, TRUE);
    va_end(list);
}
#endif
