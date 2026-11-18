#include "compiler/mwcc_core.h"
#ifndef DRIVER_CLPREFS_H
#define DRIVER_CLPREFS_H

#include "compiler/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
typedef struct PrefPanel PrefPanel;

extern PrefPanel* PrefPanel_New(const char* name, void* initdata, SInt32 initdatasize);
extern Handle PrefPanel_GetHandle(PrefPanel* panel);
extern int PrefPanel_PutHandle(PrefPanel* panel, Handle data);
#else
typedef struct PrefPanel PrefPanel;

extern PrefPanel* PrefPanel_New(const char* name, void* initdata, SInt32 initdatasize);
#if MWCCPSP
extern struct OSHandle* PrefPanel_GetHandle(PrefPanel* panel, struct OSHandle* out);
extern int PrefPanel_PutHandle(PrefPanel* panel, struct OSHandle* data);
#else
extern Handle PrefPanel_GetHandle(PrefPanel* panel);
extern int PrefPanel_PutHandle(PrefPanel* panel, Handle data);
#endif
#endif
extern Boolean Prefs_AddPanel(PrefPanel* panel);
extern PrefPanel* Prefs_FindPanel(const char* name);

#ifdef __cplusplus
}
#endif

#endif
