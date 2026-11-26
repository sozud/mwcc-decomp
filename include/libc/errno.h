#include "compiler/mwcc_core.h"
#ifndef MWCC_HOST_PROBE_ERRNO_H
#define MWCC_HOST_PROBE_ERRNO_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ThreadLocalData {
// 2.3.3's MSL: errno at +0 (_InitializeThreadData 0x402020, malloc(0x254))
#if MWCC_CORE != MWCC_CORE_B233
    struct _ThreadLocalData* next;
#endif
    int errno;
    unsigned long random_next;
    unsigned char* strtok_n;
    unsigned char* strtok_s;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    void* thread_handle;
    struct tm gmtime_tm;
    char asctime_result[26];
    char tmpnam_temp_name[512];
} _ThreadLocalData;

_ThreadLocalData* _GetThreadLocalData(void);

#define errno (_GetThreadLocalData()->errno)
#else
    void* thread_handle;
    struct tm gmtime_tm;
    char asctime_result[26];
    char tmpnam_temp_name[512];
} _ThreadLocalData;

#if MWCCPSP && defined(__MWERKS__)
// mwccpsp 3.0.1's MSL takes a "create" flag (the driver too: CLIO's errno
// reads pass 1; only its tolower is the older inline, see ctype.h)
_ThreadLocalData* _GetThreadLocalData(int create);
#define errno (_GetThreadLocalData(1)->errno)
#else
_ThreadLocalData* _GetThreadLocalData(void);
#if !MWCCPSP

#endif
#define errno (_GetThreadLocalData()->errno)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
