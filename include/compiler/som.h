#include "compiler/mwcc_core.h"
#ifndef COMPILER_SOM_H
#define COMPILER_SOM_H

#include "compiler/common.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 2)

typedef enum SOMMethodState {
#else
#ifdef __cplusplus
extern "C" {
#endif

#if !MWCCPSP // 3.0.1: natural alignment
#pragma pack(push, 2)
#endif

typedef enum SOMMethodState {
#endif
    SOMMS_Deleted,
    SOMMS_Method,
    SOMMS_Migrated
} SOMMethodState;

typedef struct SOMReleaseOrder {
    struct SOMReleaseOrder* next;
    HashNameNode* name;
    SOMMethodState state;
} SOMReleaseOrder;

struct SOMInfo {
    TypeClass* metaclass;
    Object* classdataobject;
    SOMReleaseOrder* order;
    UInt32 majorversion;
    UInt32 minorversion;
    UInt8 oidl_callstyle;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
};

#pragma pack(pop)
#else
};

#if !MWCCPSP // 3.0.1: natural alignment
#pragma pack(pop)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
