#include "compiler/mwcc_core.h"
#ifndef COMPILER_CDECL_H
#define COMPILER_CDECL_H

#include "compiler/common.h"
#include "compiler/types.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 2)
#else
#ifdef __cplusplus
extern "C" {
#endif
#if MWCCPSP

#endif
#if MWCCPSP
// 3.0.1: a type named through a base class path, queued by
// CParser_GetDeclSpecs and access-checked once the declaration is added
// (func_00500E90 -> CClass_CheckPathAccess).
typedef struct DeclAccessCheck {
    struct DeclAccessCheck* next;
    BClassList* path;
    void* unk08; // NameResult.unk1C
    HashNameNode* name;
    UInt8 access;
} DeclAccessCheck;
#endif

#pragma pack(push, 2)
#endif
struct ClassLayout {
    ObjBase** objlist;
    ObjMemberVar* vtable_ivar;
    UInt16 lex_order_count;
    UInt16 firstVirtualSlot;
    Boolean has_vtable;
};

struct DeclInfo {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    Type* thetype;
    UInt32 qual;
#if MWCC_CORE == MWCC_CORE_B52
    // b52 has 3.0.1's head (CDecl_UnpackDeclInfo 0x44A090: nspace +0x0C, name
    // +0x14, expltargs +0x34, storageclass +0x4C, has_expltargs +0x5E; sizeof 0x78)
    UInt32 qualseen;
#endif
    NameSpace* nspace;
#if MWCC_CORE == MWCC_CORE_B52
    void* unk10;
#endif
#else
    Type* thetype;
    UInt32 qual;
#if MWCCPSP
    UInt32 qualseen; // 3.0.1: cv-qualifiers already seen (CParser_GetDeclSpecs)
    NameSpace* nspace;
    void* unk10; // 3.0.1
#else
    NameSpace* nspace;
#endif
#endif
    HashNameNode* name;
    Object* resolvedObject;
    NameSpaceObjectList* resolvedObjects;
    FuncArg* parameterNames;
    NameSpace* parameterScope;
    Type* templateType;
    ENode* arrayBound;
    TemplClass* pendingClass;
    TemplArg* expltargs;
    TemplParam* templateParameters;
    struct TemplateScopeState* templateScope;
    TemplateFunction* templateFunction;
#if MWCC_CORE == MWCC_CORE_B52
    void* unk44list; // 3.0.1's
#endif
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
#if MWCCPSP
    void* unk44list; // 3.0.1: a list CDecl_TypedefDeclarator hands to func_00500E90
#endif
#endif
    UInt8 requireTemplateClassMember;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    SInt16 operator_token;
    SInt16 storageclass;
    UInt8 section;
#else
    SInt16 operator_token;
    SInt16 storageclass;
#if MWCCPSP
    UInt8 section; // 3.0.1: a byte
#else
    SInt16 section;
#endif
#endif
    UInt8 exportflags;
    Boolean hasParameterNames;
    Boolean oldStyleParameters;
    Boolean isNewExpression;
    Boolean hasArrayDimension;
    Boolean missingTypeSpecifier;
    Boolean isType;
    Boolean parserOption;
    Boolean isConstructor;
    Boolean in_friend_decl;
    Boolean isFriendDeclaration;
    Boolean is_extern_c;
    Boolean isNewTypeId;
    Boolean allowZeroLengthArray;
    Boolean allowTemplateArguments;
    Boolean has_expltargs;
    Boolean hasTypename;
    Boolean isConversionOperator;
    Boolean isTypeID;
    Boolean allowTemplateName;
    Boolean isTemplateName;
#if MWCC_CORE == MWCC_CORE_B52
    // b52's six more bytes before file (sizeof 0x78); 3.0.1 has unk64,
    // defines_tag, 2 bytes, unk68 here. Placement unverified
    Boolean unk64;
    Boolean defines_tag;
    SInt32 unk68;
#endif
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
#if MWCCPSP
    // 3.0.1: file at +0x6C, firstDeclaration at +0x78, sizeof 0x7C
    // (CDecl_FunctionDeclarator, CParser_ParseObject)
    Boolean unk64;
    Boolean defines_tag; // +0x65: set when a tag's body follows (scanenum)
    UInt8 unk66[2];
    SInt32 unk68;
#endif
#endif
    CPrepFileInfo* file;
    CPrepFileInfo* file2;
    SInt32 sourceoffset;
    Boolean firstDeclaration;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
    UInt8 interruptflags;
    ObjRegInfo reginfo;
#if MWCCPS2
#if MWCCPSP_NATIVE
    char* ps2_unk7C; // natively a pointer's room: CParser_UpdateObject reads a pointer here
#else
    UInt8 ps2_unk7C
        [4]; // mwccps2 b210: sizeof 0x80 (CDecl_UnpackDeclInfo clears 0x80); placement unverified
#endif
#endif
#endif
};

struct BigDeclInfo {
    DeclInfo declinfo;
    DeclInfo declinfo2;
    Boolean unused;
    Boolean valid;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
#if MWCCPS2
    UInt8 ps2_pad[2]; // mwccps2 b210: sizeof 0x104 (scanstructdeclarationlist clears 0x104)
#endif
#endif
};
#pragma pack(pop)

extern AccessType global_access;

extern Type* CDecl_NewStructType(SInt32 size, SInt16 align);
extern Type* CDecl_NewArrayType(Type* type, SInt32 size);
extern Type* CDecl_NewPointerType(Type* type);
extern Type* CDecl_NewRefPointerType(Type* type);
extern TypeTemplDep* CDecl_NewTemplDepType(TypeTemplDepType tdt);
extern void CDecl_SetResultReg(TypeFunc* tfunc);
extern void CDecl_SetFuncFlags(TypeFunc* tfunc, UInt32 flags);
extern void CDecl_NewConvFuncType(DeclInfo* declinfo);
extern void CDecl_CompleteType(Type* type);
extern Boolean IsCompleteType(Type* type);
extern Boolean CanAllocObject(Type* type);
extern Boolean CanCreateObject(Type* type);
extern void makethetypepointer(DeclInfo* declinfo, UInt32 qual);
extern void CDecl_AddThisPointerArgument(TypeFunc* tfunc, TypeClass* tclass);
extern void CDecl_MakePTMFuncType(TypeFunc* tfunc);
extern void CDecl_AddArgument(TypeFunc* tfunc, Type* argtype);
extern Boolean CDecl_CheckArrayIntegr(Type* type);
extern void CDecl_ParseDirectFuncDecl(DeclInfo* declinfo);
extern void makememberpointertype(DeclInfo* declinfo, TypeClass* tclass, UInt32 qual);
extern void scandeclarator(DeclInfo* declinfo);
extern void conversion_type_name(DeclInfo* declinfo);
extern Object* CDecl_GetFunctionObject(
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    DeclInfo* declinfo, NameSpace* nspace, Boolean* firstDecl, Boolean matchTemplate
);
extern void CDecl_TypedefDeclarator(DeclInfo* declinfo);
#else
    DeclInfo* declinfo, NameSpace* nspace, Boolean* firstDecl, Boolean matchTemplate
);
#if MWCCPSP
extern void CDecl_TypedefDeclarator(DeclInfo* declinfo, AccessType access); // 3.0.1
#else
extern void CDecl_TypedefDeclarator(DeclInfo* declinfo);
#endif
#endif
extern Boolean CDecl_FunctionDeclarator(
    DeclInfo* declinfo, NameSpace* nspace, Boolean allowDefinition, Boolean unused
);
extern void scandeclaratorlist(DeclInfo* declinfo);
extern void CDecl_ComputeUnderlyingEnumType(TypeEnum* tenum);
extern void scanenum(DeclInfo* declinfo);
extern void CDecl_ScanStructDeclarator(BigDeclInfo* bde);
extern void scanstruct(DeclInfo* declinfo, short structtype);
extern TypeMemberFunc* CDecl_MakeTypeMemberFunc(
    TypeFunc* tfunc, TypeClass* tclass, Boolean isStatic
);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
extern void CDecl_PackDeclInfo(PackedDeclInfo* packed, DeclInfo* declinfo);
extern void CDecl_UnpackDeclInfo(DeclInfo* declinfo, PackedDeclInfo* packed);
#if MWCC_CORE == MWCC_CORE_B52
#define CDecl_AddFriend CClass_AddFriend // b52 keeps friends in CClass.c, like 3.0.1
extern void CClass_AddFriend(TypeClass* tclass, Object* friendfunc, TypeClass* friendclass);
#else
extern void CDecl_AddFriend(TypeClass* tclass, Object* friendfunc, TypeClass* friendclass);
#endif
#else
extern void CDecl_PackDeclInfo(PackedDeclInfo* packed, DeclInfo* declinfo);
extern void CDecl_UnpackDeclInfo(DeclInfo* declinfo, PackedDeclInfo* packed);
#if MWCCPSP
#define CDecl_AddFriend CClass_AddFriend // 3.0.1 keeps friends in CClass.c
extern void CClass_AddFriend(TypeClass* tclass, Object* friendfunc, TypeClass* friendclass);
#else
extern void CDecl_AddFriend(TypeClass* tclass, Object* friendfunc, TypeClass* friendclass);
#endif
#endif
extern Boolean CDecl_CheckCtorIntegrity(FuncArg* args, TypeClass* tclass);
extern void CDecl_MakeVBaseList(TypeClass* tclass);
extern Boolean CDecl_CheckNewBase(TypeClass* tclass, TypeClass* baseclass, Boolean is_virtual);
extern TypeMemberFunc* CDecl_MakeDefaultDtorType(TypeClass* tclass, Boolean is_virtual);
extern void CDecl_CompleteClass(ClassLayout* decle, TypeClass* tclass);
extern TypeClass* CDecl_DefineClass(
    NameSpace* nspace,
    HashNameNode* name,
    TypeClass* tclass,
    short mode,
    Boolean asTypeName,
    Boolean addName
);
extern void CDecl_ParseClassDeclSpec(UInt8* declspec);
extern void CDecl_ParseClass(
    DeclInfo* declinfo, short mode, Boolean lexAfterBody, UInt8 class_declspec
);

#ifdef __cplusplus
}
#endif

#endif
