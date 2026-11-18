#include "compiler/mwcc_core.h"
#ifndef DRIVER_CLINCLUDEFILECACHE_H
#define DRIVER_CLINCLUDEFILECACHE_H

#include "compiler/common.h"
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#include "driver/oslib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void InitializeIncludeCache(void);
extern void CleanupIncludeCache(void);
extern void CacheIncludeFile(OSSpec* spec, Handle text, Boolean precompiled);
extern Handle CachedIncludeFile(OSSpec* spec, Boolean* precompiled);
extern void FreeIncludeFile(Handle text);

#ifdef __cplusplus
}
#endif
#else
#include "driver/oslib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void InitializeIncludeCache(void);
extern void CleanupIncludeCache(void);
#if MWCCPSP
extern void CacheIncludeFile(OSSpec* spec, DriverHandle text, Boolean precompiled);
extern DriverHandle CachedIncludeFile(OSSpec* spec, Boolean* precompiled);
extern void FreeIncludeFile(DriverHandle text);
#else
extern void CacheIncludeFile(OSSpec* spec, Handle text, Boolean precompiled);
extern Handle CachedIncludeFile(OSSpec* spec, Boolean* precompiled);
extern void FreeIncludeFile(Handle text);
#endif

#ifdef __cplusplus
}
#endif
#endif

#endif
