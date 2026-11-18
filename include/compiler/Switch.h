#include "compiler/mwcc_core.h"
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifndef COMPILER_SWITCH_H
#define COMPILER_SWITCH_H

#include "compiler/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// b69: case ranges (`case a ... b`): scancase (0x471210) and
// CInline_UnpackSwitch (0x463D00) allocate 0x28 bytes, min at +8, max at
// +0x18. SwitchInfo is 0xC bytes (UnpackSwitch), exprtype at +8: no
// template-dependent case list
struct SwitchCase {
    struct SwitchCase* next;
    struct CLabel* label;
    CInt64 min;
    CInt64 max;
};

struct SwitchInfo {
    struct SwitchCase* cases;
#if MWCC_CORE == MWCC_CORE_B52
    void* x4;
#endif
    struct CLabel* defaultlabel;
    Type* exprtype;
};

extern ObjectList* switchtables;

extern void switchstatement(ENode* expr, struct SwitchInfo* info);
extern void dumpswitchtables(Object* funcobj);

#ifdef __cplusplus
}
#endif

#endif
#else
#ifndef COMPILER_SWITCH_H
#define COMPILER_SWITCH_H

#include "compiler/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MWCCPSP
// 3.0.1 (scancase 0x46D1E0, the case statement 0x46CF30): cases are ranges
// (`case a ... b`), and case labels whose values depend on template
// arguments are kept on a separate list until instantiation.
struct SwitchCase {
    struct SwitchCase* next;
    struct CLabel* label;
    CInt64 min;
    CInt64 max;
};

struct SwitchInfo {
    struct SwitchCase* cases;
    void* depcases;
    struct CLabel* defaultlabel;
    Type* exprtype;
};
#else
struct SwitchCase {
    struct SwitchCase* next;
    struct CLabel* label;
    CInt64 min;
};

struct SwitchInfo {
    struct SwitchCase* cases;
    struct CLabel* defaultlabel;
    Type* exprtype;
};
#endif

extern ObjectList* switchtables;

extern void switchstatement(ENode* expr, struct SwitchInfo* info);
extern void dumpswitchtables(Object* funcobj);

#ifdef __cplusplus
}
#endif

#endif
#endif
