#include "driver/MemUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MWCC_CORE == MWCC_CORE_B233
void* MEMUTILS_CC xmalloc(const char* what, int size) {
#else
void* __stdcall xmalloc(const char* what, int size) {
#endif
    void* ret;

    ret = malloc(size ? size : 1);
    if (!ret) {
        fprintf(
            stderr,
            "*** Out of memory when allocating %d bytes%s%s",
            size,
            what ? " for " : "",
            what ? what : ""
        );
        exit(-23);
        return NULL;
    }

    return ret;
}

#if MWCC_CORE == MWCC_CORE_B233
void* MEMUTILS_CC xcalloc(const char* what, int size) {
#else
void* __stdcall xcalloc(const char* what, int size) {
#endif
    void* ret;

    ret = xmalloc(what, size);
    memset(ret, 0, size);
    return ret;
}

#if MWCC_CORE == MWCC_CORE_B233
void* MEMUTILS_CC xrealloc(const char* what, void* old, int size) {
#else
void* __stdcall xrealloc(const char* what, void* old, int size) {
#endif
    void* ret;

    ret = realloc(old, size ? size : 1);
    if (!ret) {
        fprintf(
            stderr,
            "*** Out of memory when resizing buffer to %d bytes%s%s",
            size,
            what ? " for " : "",
            what ? what : ""
        );
        exit(-23);
        return NULL;
    }

    return ret;
}

#if MWCC_CORE == MWCC_CORE_B233
char* MEMUTILS_CC xstrdup(const char* str) {
#else
char* __stdcall xstrdup(const char* str) {
#endif
    return strcpy(xmalloc(NULL, strlen(str) + 1), str);
}

#if MWCC_CORE == MWCC_CORE_B233
void MEMUTILS_CC xfree(void* ptr) {
#else
void __stdcall xfree(void* ptr) {
#endif
    if (ptr) {
        free(ptr);
    }
}
