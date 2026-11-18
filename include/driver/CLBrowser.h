#include "compiler/mwcc_core.h"
#ifndef DRIVER_CLBROWSER_H
#define DRIVER_CLBROWSER_H

#include "compiler/common.h"
#include "driver/oslib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int Browser_Initialize(OSHandle* browsetableptr);
extern int Browser_Terminate(OSHandle* browsetableptr);
extern int Browser_SearchFile(OSHandle* browsetable, const char* fullpath, SInt16* ID);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
extern int Browser_SearchAndAddFile(OSHandle* browsetable, const char* fullpath, SInt16* ID);
extern int Browser_PackBrowseFile(
    Handle browsedata, OSHandle* browsetable, OSHandle* browsefileptr
#else
extern int Browser_SearchAndAddFile(OSHandle* browsetable, const char* fullpath, SInt16* ID);
extern int Browser_PackBrowseFile(
#if MWCCPSP
    DriverHandle browsedata, OSHandle* browsetable, OSHandle* browsefileptr
#else
    Handle browsedata, OSHandle* browsetable, OSHandle* browsefileptr
#endif
#endif
);

#ifdef __cplusplus
}
#endif

#endif
