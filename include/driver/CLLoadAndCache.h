#include "compiler/mwcc_core.h"
#ifndef DRIVER_CLLOADANDCACHE_H
#define DRIVER_CLLOADANDCACHE_H

#include "compiler/common.h"
#include "driver/oslib.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifdef __cplusplus
extern "C" {
#endif

extern short FixTextHandle(Handle txt);
extern int LoadAndCacheFile(OSSpec* spec, Handle* texthandle, Boolean* precomp);
extern void CopyFileText(Handle src, char** text, SInt32* textsize);
#else
#ifdef __cplusplus
extern "C" {
#endif

#if MWCCPSP
extern short FixTextHandle(DriverHandle txt);
extern int LoadAndCacheFile(OSSpec* spec, DriverHandle* texthandle, Boolean* precomp);
extern void CopyFileText(DriverHandle src, char** text, SInt32* textsize);
#else
extern short FixTextHandle(Handle txt);
extern int LoadAndCacheFile(OSSpec* spec, Handle* texthandle, Boolean* precomp);
extern void CopyFileText(Handle src, char** text, SInt32* textsize);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
