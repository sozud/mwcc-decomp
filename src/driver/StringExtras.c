#include "driver/StringExtras.h"
#include <ctype.h>
#include <string.h>

char* __stdcall strcatn(char* d, const char* s, SInt32 max) {
    char* p;

    p = d + strlen(d);
    while (*s && (p - d) + 1 < max) {
        *(p++) = *(s++);
    }

    *p = 0;
    return d;
}

char* __stdcall strcpyn(char* d, const char* s, SInt32 len, SInt32 max) {
    char* p;

    p = d;
    while (len-- && *s && (p - d) + 1 < max) {
        *(p++) = *(s++);
    }

    *p = 0;
    return d;
}

#if MWCC_CORE == MWCC_CORE_B233
int USTRCMP_CC ustrcmp(const char* src, const char* dst) {
#else
int __stdcall ustrcmp(const char* src, const char* dst) {
#endif
    int diff;

    do {
        diff = tolower(*src) - tolower(*(dst++));
        if (diff) {
            return diff;
        }
    } while (*(src++));

    return 0;
}

int __stdcall ustrncmp(const char* src, const char* dst, UInt32 len) {
    int diff;

    while (len-- != 0) {
        diff = tolower(*src) - tolower(*(dst++));
        if (diff) {
            return diff;
        }
        if (!*(src++)) {
            return 0;
        }
    }

    return 0;
}
