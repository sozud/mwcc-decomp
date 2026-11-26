#ifndef COMPILER_SCOPES_H
#define COMPILER_SCOPES_H

#include "compiler/common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NameSpace {
    NameSpace* parent;
    HashNameNode* name;
    NameSpaceList* usings;
    TypeClass* theclass;
#if MWCC_CORE != MWCC_CORE_B233
    NameSpaceName* tparams;
#endif
    union {
        NameSpaceName** hash;
        NameSpaceName* list;
    } data;
    UInt32 names;
    Boolean is_hash;
    Boolean is_global;
    Boolean is_unnamed;
    Boolean is_templ;
#if MWCC_CORE == MWCC_CORE_B233
    // 2.3.3: is_hash at +0x18, is_unnamed at +0x1A (dispmap.py: 0x1C/0x1E ->
    // 0x18/0x1A in CPrec_GetNameSpacePatch, CParser_ParseNameSpace): one pointer
    // fewer before data; tparams is ours alone (which pointer is unverified)
    NameSpaceName* tparams;
#endif
};
struct NameSpaceList {
    NameSpaceList* next;
    NameSpace* nspace;
};
struct NameSpaceObjectList {
    NameSpaceObjectList* next;
    ObjBase* object;
};
struct NameSpaceName {
    NameSpaceName* next;
    HashNameNode* name;
    NameSpaceObjectList first;
};

#ifdef __cplusplus
}
#endif

#endif
