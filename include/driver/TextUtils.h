#include "compiler/mwcc_core.h"
#ifndef DRIVER_TEXTUTILS_H
#define DRIVER_TEXTUTILS_H

#include "compiler/common.h"
#include "driver/oslib.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifdef __cplusplus
extern "C" {
#endif

extern StringPtr c2pstr(char* s);
extern char* __stdcall p2cstr(StringPtr s);
extern void __stdcall GetIndString(StringPtr theString, SInt16 strListID, SInt16 index);
#else
#ifdef __cplusplus
extern "C" {
#endif

#if MWCCPSP
extern StringPtr __stdcall c2pstr(char* s);
#else
extern StringPtr c2pstr(char* s);
#endif
extern char* __stdcall p2cstr(StringPtr s);
extern void __stdcall GetIndString(StringPtr theString, SInt16 strListID, SInt16 index);
#endif
extern char* __stdcall getindstring(char* theString, SInt16 strListID, SInt16 index);

#ifdef __cplusplus
}
#endif

#endif
