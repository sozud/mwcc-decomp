#include "compiler/mwcc_core.h"
#ifndef COMPILER_OBJECTS_H
#define COMPILER_OBJECTS_H

#include "compiler/common.h"
#include "compiler/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OBJ_BASE(obj) ((ObjBase*)(obj))
#define OBJ_ENUM_CONST(obj) ((ObjEnumConst*)(obj))
#define OBJ_TYPE(obj) ((ObjType*)(obj))
#define OBJ_MEMBER_VAR(obj) ((ObjMemberVar*)(obj))
#define OBJ_TYPE_TAG(obj) ((ObjTypeTag*)(obj))
#define OBJ_NAMESPACE(obj) ((ObjNameSpace*)(obj))
#define OBJ_MEMBER_VAR_PATH(obj) ((ObjMemberVarPath*)(obj))
#define OBJECT(obj) ((Object*)(obj))
enum {
    OT_ENUMCONST,
    OT_TYPE,
    OT_TYPETAG,
    OT_NAMESPACE,
    OT_MEMBERVAR,
    OT_OBJECT,
    OT_ILLEGAL
};
enum {
    DDATA,
    DLOCAL,
    DABSOLUTE,
    DFUNC,
    DVFUNC,
    DINLINEFUNC,
    DALIAS,
    DEXPR,
    DLABEL
};
typedef UInt16 Section;
#define SECT_DEFAULT 0
#define SECT_TEXT 1
#define SECT_DATA 2
#define SECT_OBJC_CAT_CLS_METH 7
#define SECT_OBJC_CAT_INST_METH 8
#define SECT_OBJC_STRING_OBJECT 9
#define SECT_OBJC_CSTRING_OBJECT 10
#define SECT_OBJC_MESSAGE_REFS 11
#define SECT_OBJC_SEL_FIXUP 12
#define SECT_OBJC_CLS_REFS 13
#define SECT_OBJC_CLASS 14
#define SECT_OBJC_META_CLASS 15
#define SECT_OBJC_CLS_METH 16
#define SECT_OBJC_INST_METH 17
#define SECT_OBJC_PROTOCOL 18
#define SECT_OBJC_CLASS_NAMES 19
#define SECT_OBJC_METH_VAR_TYPES 20
#define SECT_OBJC_METH_VAR_NAMES 21
#define SECT_OBJC_CATEGORY 22
#define SECT_OBJC_CLASS_VARS 23
#define SECT_OBJC_INSTANCE_VARS 24
#define SECT_OBJC_MODULE_INFO 25
#define SECT_OBJC_MSYMBOLS 26
#define SECT_CONST_TYPE 0x20
#define SECT_SDATA_TYPE 0x21
#define SECT_SCONST_TYPE 0x22
enum {
    OBJECT_USED = 1,
    OBJECT_REFERENCED = 2,
    OBJECT_DEFINED = 4,
    OBJECT_LAZY = 8,
    OBJECT_INTERNAL = 0x10,
    OBJECT_IMPORT = 0x20,
    OBJECT_EXPORT = 0x40,
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    OBJECT_LIB_EXPORT = 0x60
};
#pragma pack(push, 2)
#else
    OBJECT_LIB_EXPORT = 0x60
};
#if MWCCPSP
// 3.0.1: natural alignment, and every object's access is a single byte
typedef UInt8 ObjAccess;
#else
#pragma pack(push, 2)
#endif
#endif
struct ObjectList {
    ObjectList* next;
    Object* object;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjBase {
    UInt8 otype;
    AccessType access;
#else
struct ObjBase {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjEnumConst {
    UInt8 otype;
    AccessType access;
#else
struct ObjEnumConst {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    ObjEnumConst* next;
    HashNameNode* name;
    Type* type;
    CInt64 val;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjNameSpace {
    UInt8 otype;
    AccessType access;
#else
struct ObjNameSpace {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    NameSpace* nspace;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjType {
    UInt8 otype;
    AccessType access;
#else
struct ObjType {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    Type* type;
    UInt32 qual;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjMemberVar {
    UInt8 otype;
    AccessType access;
#else
struct ObjMemberVar {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    Boolean anonunion;
    Boolean has_path;
    ObjMemberVar* next;
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    UInt32 offset;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjTypeTag {
    UInt8 otype;
    AccessType access;
#else
struct ObjTypeTag {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    Type* type;
};
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct ObjMemberVarPath {
    UInt8 otype;
    AccessType access;
#else
struct ObjMemberVarPath {
    UInt8 otype;
#if MWCCPSP
    ObjAccess access;
#else
    AccessType access;
#endif
#endif
    Boolean anonunion;
    Boolean has_path;
    ObjMemberVar* next;
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    UInt32 offset;
    BClassList* path;
};
struct InlineXRef {
    struct InlineXRef* next;
    Object* object;
    UInt16 xrefoffset;
    UInt16 numxrefs;
    struct {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        UInt32 offset;
        SInt32 varoffset;
    } xref[1];
};
struct Object {
    UInt8 otype;
    // b69 (as 3.0.1): access, datatype and section are bytes (CParser_UpdateObject
    // stores the section at +3; type at +0xC); sizeof(Object) is 0x40
    // (CParser_NewLocalDataObject), the value at +0x28 is a 16-byte CInt64 and a
    // data object's link name is at +0x38 (CMangler_GetLinkName)
    UInt8 access;
    UInt8 datatype;
    UInt8 section;
#else
        UInt32 offset;
        SInt32 varoffset;
    } xref[1];
};
#if MWCCPSP
#pragma pack(push, 2) // Object and VarInfo below are laid out under it
#endif
struct Object {
    UInt8 otype;
#if MWCCPSP
    // 3.0.1: access and section are single bytes (CParser_NewObject: access
    // at +1, section at +3; sizeof(Object) is 0x40)
    UInt8 access;
    UInt8 datatype;
    UInt8 section;
#else
    AccessType access;
    UInt8 datatype;
    UInt16 section;
#endif
#endif
    NameSpace* nspace;
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    SInt16 sclass;
    UInt8 flags;
#if MWCC_CORE != MWCC_CORE_B233
    struct ExtendedParam* extParam;
#endif
    union {
        struct Object* toc;
        struct DwarfNode* pendingEntry;
    };
    union {
        struct DwarfSymbol* symbol;
        struct DwarfNode* entry;
    } debugInfo;
    struct VarRecord* varptr;
    union {
        struct {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
            union {
                CInt64 intconst;
                Float* floatconst;
                char* string;
                struct {
                    char* data;
                    SInt32 size;
                } switchtable;
            } u;
            // b69: no register info (CParser_UpdateObject copies none) and, as
            // in GC 2.0, no islocalstatic; info at +0x3C is unverified
            HashNameNode* linkname;
#if MWCC_CORE != MWCC_CORE_B233 // 2.3.3: a data object ends with its link name (Object is 0x30)
            union {
                VarInfo* info;
                struct InterruptGenerationRecord* interruptInfo;
            };
#endif
#else
            union {
                CInt64 intconst;
                Float* floatconst;
                char* string;
                struct {
                    char* data;
                    SInt32 size;
                } switchtable;
            } u;
#if MWCCPS2
            // mwccps2 b210: sizeof(Object) is 0x44; the value is a 16-byte
            // CInt64, so CPrec_GetObjectPatch and CMangler_GetLinkName read a
            // data link name at +0x38 (a function's stays at +0x30) and
            // add_aliased_locals_to_worst_case reads islocalstatic at +0x40.
            // info at +0x3c is unverified.
            HashNameNode* linkname;
            union {
                VarInfo* info;
                struct InterruptGenerationRecord* interruptInfo;
            };
            Boolean islocalstatic;
            UInt8 ps2_pad41[3]; // CParser_NewObject allocates 0x44
#elif MWCCPSP
            // 3.0.1: the link name follows the value (CMangler_GetLinkName
            // reads it at +0x30, a switch table's size is at +0x2C); there
            // is no register info (CParser_UpdateObject does not copy it).
            HashNameNode* linkname;
            Boolean islocalstatic; // +0x34 (add_aliased_locals_to_worst_case)
            union {
                VarInfo* info;
                struct InterruptGenerationRecord* interruptInfo;
            };
#else
            union {
                VarInfo* info;
                struct InterruptGenerationRecord* interruptInfo;
            };
#if VERSION < VERSION_GC_2_5
            ObjRegInfo reginfo;
#endif
            HashNameNode* linkname;
#if VERSION >= VERSION_GC_2_5
            ObjRegInfo reginfo;
#endif
            Boolean islocalstatic;
#endif
#endif
        } data;
        struct {
            union {
                TemplateFunction* templ;
                struct CI_FuncData* ifuncdata;
                SInt32 intrinsicid;
            } u;
            DefArgCtorInfo* defargdata;
            HashNameNode* linkname;
#if MWCC_CORE != MWCC_CORE_B233
            // 2.3.3 has neither: template function instances are not linked
            // from their objects (CMangler_FunctionLinkName 0x454290 reads no
            // +0x30; CTempl_GetCreateFuncInstance stores none) and there is no
            // pointer analysis (no IroPointerAnalysis.c, no
            // opt_pointer_analysis pragma)
            TemplFuncInstance* inst;
            struct PTFList* ptfList;
            ObjectList* argList;
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        } func;
        struct {
            VarInfo* info;
            SInt32 uid;
            union {
                struct Object* realObj;
                SInt32 offset; // a local's frame offset (MIPS back end, as 3.0.1)
            };
#else
#if !MWCCPSP // 3.0.1: gone, sizeof(Object) is 0x40
            UInt8 interruptflags;
#endif
        } func;
        struct {
            VarInfo* info;
            SInt32 uid;
#if MWCCPSP
            union {
                struct Object* realObj;
                SInt32 offset; // 3.0.1: a local's frame offset (0x4A4530)
            };
#else
            struct Object* realObj;
#endif
#endif
        } var;
        struct {
            UInt8* data;
            SInt32 size;
            InlineXRef* xrefs;
        } ifunc;
        struct {
            Object* object;
            BClassList* member;
            SInt32 offset;
        } alias;
        SInt16 intrinsic;
        ENode* expr;
        UInt32 address;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
#if MWCCPSP
        struct {
            struct Object* function; // a label object's function (IRO_CollectAddressedLabels)
        } label;
#endif
#endif
    } u;
    // 2.3.3: Object is 0x30 bytes (CParser_NewLocalDataObject/NewFunctionObject
    // allocate 0x30) with u at +0x24 (dispmap.py: 0x28 -> 0x24) and a link name at
    // +0x2C (CMangler_GetLinkName 0x4544B0); no extParam (pointer analysis only)
};
#if MWCC_CORE == MWCC_CORE_B233
// fields 2.3.3's Object does not have: the code that would use them reads
// NULL (no template instance link, no data VarInfo), and the pointer analysis
// (not in 2.3.3, never enabled: no pragma) stops in PA_NoObjectField
#define OBJ_FUNC_INST(o) ((TemplFuncInstance*)0)
#define OBJ_DATA_INFO(o) ((VarInfo*)0)
#define OBJ_ISLOCALSTATIC(o) 0
extern void* PA_NoObjectField(void);
#define OBJ_PTFLIST(o) (*(struct PTFList**)PA_NoObjectField())
#define OBJ_ARGLIST(o) (*(ObjectList**)PA_NoObjectField())
#define OBJ_EXTPARAM(o) (*(struct ExtendedParam**)PA_NoObjectField())
#else
#define OBJ_FUNC_INST(o) ((o)->u.func.inst)
#define OBJ_DATA_INFO(o) ((o)->u.data.info)
#define OBJ_ISLOCALSTATIC(o) ((o)->u.data.islocalstatic)
#define OBJ_PTFLIST(o) ((o)->u.func.ptfList)
#define OBJ_ARGLIST(o) ((o)->u.func.argList)
#define OBJ_EXTPARAM(o) ((o)->extParam)
#endif
struct ObjectTemplated {
    Object object;
    Object* parent;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
#if MWCCPSP
    // 3.0.1: the instance stack when a templated function was first referenced
    // (CInline_ObjectAddrRef; sizeof is 0x48 in CTemplClass_CopyObject)
    TemplStack* inststack;
#endif
#endif
};
struct VarInfo {
    struct Object* func;
    SInt32 usage;
    TStreamElement deftoken;
    SInt16 varnumber;
    Boolean noregister;
    Boolean used;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    // b69's VarInfo is 0x34 bytes (CodeGen_GetNewVarInfo 0x4337F0 allocates
    // and clears 0x34); 0x434410 (func_00438160) tests +0x2D, stores the
    // class at +0x2F (5 or 0) and the register at +0x30, 0x4341E0 the same
#if MWCC_CORE == MWCC_CORE_B233
    // 2.3.3: 0x2C bytes (CodeGen_GetNewVarInfo lalloc(0x2C)); b69's four
    // bytes are flags (|= at +0x25) and the register class (+0x26), with the
    // register at +0x28
    union {
        UInt8 unk24[4];
        struct {
            UInt8 unk24_0;
            UInt8 flags;
            UInt8 rclass;
            UInt8 unk24_3;
        };
    };
#else
    UInt8 unk24[2]; // +0x2C; [1] the VU0 flag (gcc_replace_arg_st_asm, +0x2D)
    UInt8 flags;
    UInt8 rclass;
#endif
#else
#if MWCCPS2
    UInt8 unk24
        [4]; // mwccps2 b210: rclass +0x31, reg +0x32 (func_0049DE40), 2 bytes more than mwccpsp
#elif MWCCPSP
    UInt8 unk24
        [2]; // 3.0.1: flags at +0x26, rclass +0x27, reg +0x28, regHi +0x2A (Coloring.c, RegClasses.c)
#endif
    UInt8 flags;
    UInt8 rclass;
#endif
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    SInt16 reg;
    SInt16 regHi;
};
// b210's register-bound data objects read a class at +0x2E and a byte at
// +0x2F of the VarInfo at Object+0x3C; b69 has no such objects (nothing
// sets u.data.info), so these paths never run
#define VI_B210_CLASS(vi) ((vi)->rclass)
#define VI_B210_2F(vi) ((vi)->flags)
#pragma pack(pop)
#define PS2_VARINFO_AT(vi, x86off) ((char*)(vi) + (x86off))
#else
    SInt16 reg;
    SInt16 regHi;
#if MWCCPS2
    UInt8 unk2C[2]; // mwccps2 b210: CodeGen_GetNewVarInfo allocates 0x38
#elif MWCCPSP
    UInt8 unk2C[4]; // 3.0.1: CodeGen_GetNewVarInfo allocates 0x30
#endif
};
#pragma pack(pop)
#if MWCCPS2
// b210 code addresses VarInfo's tail by its x86 offsets (0x2C..0x37: the
// register-variable record's class at +0x2E, register at +0x32); natively
// the struct has 8-byte pointers, so they are taken from unk24
#if MWCCPSP_NATIVE
#define PS2_VARINFO_AT(vi, x86off) ((char*)(vi) + offsetof(VarInfo, unk24) + ((x86off) - 0x2C))
#else
#define PS2_VARINFO_AT(vi, x86off) ((char*)(vi) + (x86off))
#endif
#endif
#endif
enum {
    VarInfoFlag_ParamArea = 1,
    VarInfoFlag_InReg = 2,
    VarInfoFlag_RegPair = 4,
    VarInfoFlag_TempInUse = 0x20,
    VarInfoFlag_AsmNamed = 0x40,
    VarInfoFlag_HasMemory = 0x80
};

#define OBJECT_TEMPL(obj) ((ObjectTemplated*)(obj))

HEADER_INLINE Boolean Inline_IsObjectData(Object* object) {
    return object->datatype == DDATA || object->datatype == DABSOLUTE;
}

#ifdef __cplusplus
}
#endif

#endif
