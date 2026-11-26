#include "compiler/mwcc_core.h"
#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H
#include "version.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
// the MIPS back end (src/psp/backend) marks functions __declspec(noinline)
// for CW 9.2; CW Pro 6 has no such declspec
#define BE_NOINLINE

#endif
#ifdef __cplusplus
extern "C" {
#endif

struct CInlineBackendOptions;
struct MachineInfo;
struct DwarfSym;
typedef struct TemplStack TemplStack;
typedef struct Alias Alias;
typedef struct ArgMatch ArgMatch;
typedef struct IAEffect IAEffect;
typedef struct IAMnemonic IAMnemonic;
typedef struct IAEffects IAEffects;
typedef struct BClassList BClassList;
typedef struct BE_SymNode BE_SymNode;
typedef struct Conversion Conversion;
typedef struct BitVector BitVector;
typedef struct CPrepFileInfo CPrepFileInfo;
typedef struct BufferUpdate BufferUpdate;
typedef struct TStreamElement TStreamElement;
typedef struct ClassFriend ClassFriend;
typedef struct DeclInfo DeclInfo;
typedef struct ExceptionAction ExceptionAction;
typedef struct CGList CGList;
typedef struct CI_SwitchCase CI_SwitchCase;
typedef struct CI_FuncData CI_FuncData;
typedef struct CI_Var CI_Var;
typedef struct CInt64 CInt64;
typedef struct CLabel CLabel;
typedef struct BasicInductionVar BasicInductionVar;
typedef struct CNameNode CNameNode;
typedef struct OLinkList OLinkList;
typedef struct CompilerLinkerOptions CompilerLinkerOptions;
typedef struct ParserTryBlock ParserTryBlock;
typedef struct ObjType ObjType;
typedef struct ObjTypeTag ObjTypeTag;
typedef struct ObjMemberVarPath ObjMemberVarPath;
typedef struct CI_Action CI_Action;
typedef struct CompilerLinkerParamBlk CompilerLinkerParamBlk;
typedef struct NameResult NameResult;
typedef struct CScopeSave CScopeSave;
typedef struct TemplArg TemplArg;
typedef struct Candidate Candidate;
typedef struct ClassLayout ClassLayout;
typedef struct ClassList ClassList;
typedef struct TemplPartialSpec TemplPartialSpec;
typedef struct PropInfo PropInfo;
typedef struct InductionVar InductionVar;
typedef struct UseOrDef UseOrDef;
typedef struct RegUseOrDef RegUseOrDef;
typedef struct ObjectUseDef ObjectUseDef;
typedef struct InstrList InstrList;
typedef struct BlockList BlockList;
typedef struct ComparisonValues ComparisonValues;
typedef struct ConIteratorList ConIteratorList;
typedef struct ConIterator ConIterator;
typedef struct ConversionIterator ConversionIterator;
typedef struct CtorChain CtorChain;
typedef struct DWInfo DWInfo;
typedef struct DefArgCtorInfo DefArgCtorInfo;
typedef struct DwarfFixup DwarfFixup;
typedef struct DwarfFunctionState DwarfFunctionState;
typedef struct DwarfLocationOperand DwarfLocationOperand;
typedef struct DwarfNode DwarfNode;
typedef struct DwarfRef DwarfRef;
typedef struct DwarfStateList DwarfStateList;
typedef struct DwarfSymbol DwarfSymbol;
typedef struct ENode ENode;
typedef struct ENodeList ENodeList;
typedef struct TemplateAction TemplateAction;
typedef struct EMemberInfo EMemberInfo;
typedef struct PCode PCode;
typedef struct ExceptSpecList ExceptSpecList;
typedef struct PCodeBlock PCodeBlock;
typedef struct EntryPoint EntryPoint;
typedef struct ExceptionTableHeader ExceptionTableHeader;
typedef struct FileOffsetInfo FileOffsetInfo;
typedef struct PCodeArg PCodeArg;
typedef struct Float Float;
typedef struct FuncArg FuncArg;
typedef struct GList GList;
typedef struct HashNameNode HashNameNode;
typedef struct SwitchInfo SwitchInfo;
typedef struct IROAddrRecord IROAddrRecord;
typedef struct IRODef IRODef;
typedef struct IROElmList IROElmList;
typedef struct IROExpr IROExpr;
typedef struct IROLinear IROLinear;
typedef struct IROList IROList;
typedef struct IROListNode IROListNode;
typedef struct IROLoop IROLoop;
typedef struct IROLoopInd IROLoopInd;
typedef struct IRONode IRONode;
typedef struct IROUse IROUse;
typedef struct CI_Statement CI_Statement;
typedef struct IARegister IARegister;
typedef struct CI_Switch CI_Switch;
typedef struct InlineXRef InlineXRef;
typedef struct InterruptGenerationRecord InterruptGenerationRecord;
typedef struct InterruptList InterruptList;
typedef struct CScopeNSIterator CScopeNSIterator;
typedef struct Loop Loop;
typedef struct Macro Macro;
typedef struct TokenStack TokenStack;
typedef struct FuncMatchArgs FuncMatchArgs;
typedef struct BigDeclInfo BigDeclInfo;
typedef struct NameSpace NameSpace;
typedef struct NameSpaceList NameSpaceList;
typedef struct NameSpaceName NameSpaceName;
typedef struct NameSpaceObjectList NameSpaceObjectList;
typedef struct PackedDeclInfo PackedDeclInfo;
typedef struct ObjBase ObjBase;
typedef struct ObjCCategory ObjCCategory;
typedef struct ObjCInfo ObjCInfo;
typedef struct ObjCMethod ObjCMethod;
typedef struct ObjCMethodArg ObjCMethodArg;
typedef struct ObjCMethodList ObjCMethodList;
typedef struct ObjCNamedArg ObjCNamedArg;
typedef struct ObjCProtocol ObjCProtocol;
typedef struct ObjCProtocolList ObjCProtocolList;
typedef struct ObjCSelector ObjCSelector;
typedef struct ObjEnumConst ObjEnumConst;
typedef struct ObjGenRelocation ObjGenRelocation;
typedef struct ObjGenSection ObjGenSection;
typedef struct ObjGenSymbolLink ObjGenSymbolLink;
typedef struct ObjMemberVar ObjMemberVar;
typedef struct ObjNameSpace ObjNameSpace;
typedef struct Object Object;
typedef struct ObjectList ObjectList;
typedef struct Operand Operand;
typedef struct PCLink PCLink;
typedef struct PCodeLabel PCodeLabel;
typedef struct InitExpr InitExpr;
typedef struct PendingObject PendingObject;
typedef struct TemplParam TemplParam;
typedef struct TemplateFriend TemplateFriend;
typedef struct TemplateMember TemplateMember;
typedef struct TokenStream TokenStream;
typedef struct QualNode QualNode;
typedef struct IROAssign IROAssign;
typedef struct TypeBitfield TypeBitfield;
typedef struct SOMInfo SOMInfo;
typedef struct NameSpaceLookupList NameSpaceLookupList;
typedef struct CScopeObjectIterator CScopeObjectIterator;
typedef struct SectionAttributeNode SectionAttributeNode;
typedef struct SectionRec SectionRec;
typedef struct SectionSymbolAttributes SectionSymbolAttributes;
typedef struct Statement Statement;
typedef struct DeclThing DeclThing;
typedef struct StructMember StructMember;
typedef struct SwitchCase SwitchCase;
typedef struct CodeLabelList CodeLabelList;
typedef struct TemplParamID TemplParamID;
typedef struct PSpecList PSpecList;
typedef struct TypeDeduce TypeDeduce;
typedef struct TemplateFunction TemplateFunction;
typedef struct DefAction DefAction;
typedef struct DeduceInfo DeduceInfo;
typedef struct ObjectTemplated ObjectTemplated;
typedef struct TemplateScopeState TemplateScopeState;
typedef struct TemplFuncInstance TemplFuncInstance;
typedef struct Type Type;
typedef struct TypeClass TypeClass;
typedef struct TemplClass TemplClass;
typedef struct TemplClassInst TemplClassInst;
typedef struct TypeEnum TypeEnum;
typedef struct TypeFunc TypeFunc;
typedef struct TypeIntegral TypeIntegral;
typedef struct TypeMemberFunc TypeMemberFunc;
typedef struct TypeMemberPointer TypeMemberPointer;
typedef struct TypePointer TypePointer;
typedef struct TypeObjCID TypeObjCID;
typedef struct TypeList TypeList;
typedef struct TypeStruct TypeStruct;
typedef struct TypeTemplDep TypeTemplDep;
typedef struct VClassList VClassList;
typedef struct VTable VTable;
typedef struct VarInfo VarInfo;
typedef struct VarRecord VarRecord;
typedef struct WeirdOperand WeirdOperand;
typedef union ENodeUnion ENodeUnion;
typedef union FloatStorage FloatStorage;
typedef union MWVector128 MWVector128;

#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifdef __MWERKS__
#define HEADER_INLINE inline
#else
#define HEADER_INLINE static inline
#endif
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef short SInt16;
#ifdef __LP64__
// native 64-bit build: the original's long is 32 bits
typedef unsigned int UInt32;
typedef int SInt32;
#else
typedef unsigned long UInt32;
typedef long SInt32;
#endif
#else
#ifdef __MWERKS__
#define HEADER_INLINE inline
#else
#define HEADER_INLINE static inline
#endif
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef short SInt16;
#if MWCCPSP
#ifdef __LP64__
// native 64-bit build: the original's long is 32 bits
typedef unsigned int UInt32;
typedef int SInt32;
#else
typedef unsigned long UInt32;
typedef long SInt32;
#endif
#else
typedef unsigned long UInt32;
typedef long SInt32;
#endif
#endif
typedef unsigned char Boolean;

#ifdef __MWERKS__
typedef short VarArgShort;
#else
typedef int VarArgShort;
#endif

typedef char* Ptr;
typedef Ptr* Handle;

typedef enum TemplParamType {
    TPT_TYPE = 0,
    TPT_NONTYPE,
    TPT_TEMPLATE
} TemplParamType;

struct TemplParamID {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    UInt16 index;
    UInt8 nindex;
    TemplParamType type;
#else
    UInt16 index;
    UInt8 nindex;
#if MWCCPSP
    UInt8 type; // a byte in 3.0.1 (TypeTemplDep.u.pid.type at +0xF)
#else
    TemplParamType type;
#endif
#endif
};

typedef enum AccessType {
    ACCESSPUBLIC,
    ACCESSPRIVATE,
    ACCESSPROTECTED,
    ACCESSNONE
} AccessType;
enum {
    Q_CONST = 1,
    Q_VOLATILE = 2,
    Q_CV = Q_CONST | Q_VOLATILE,
    Q_ASM = 4,
    Q_PASCAL = 8,
    Q_INLINE = 0x10,
    Q_REFERENCE = 0x20,
    Q_EXPLICIT = 0x40,
    Q_MUTABLE = 0x80,
    Q_VIRTUAL = 0x100,
    Q_FRIEND = 0x200,
    Q_IN = 0x400,
    Q_OUT = 0x800,
    Q_INOUT = 0x1000,
    Q_BYCOPY = 0x2000,
    Q_BYREF = 0x4000,
    Q_ONEWAY = 0x8000,
    Q_INLINE_DATA = 0x10000,
    Q_IMPLICIT_WEAK = 0x20000,
    Q_WEAK = 0x40000,
    Q_MANGLE_NAME = 0x80000,
    Q_IS_OBJC_ID = 0x100000,
    Q_RESTRICT = 0x200000,
    Q_IS_TEMPLATED = 0x400000,
    Q_TENTATIVE = 0x1000000,
    Q_ALIGNED_1 = 0x2000000,
    Q_ALIGNED_2 = 0x4000000,
    Q_ALIGNED_4 = 0x6000000,
    Q_ALIGNED_8 = 0x8000000,
    Q_ALIGNED_16 = 0xA000000,
    Q_ALIGNED_32 = 0xC000000,
    Q_ALIGNED_64 = 0x10000000,
    Q_ALIGNED_128 = 0x12000000,
    Q_ALIGNED_256 = 0x14000000,
    Q_ALIGNED_512 = 0x16000000,
    Q_ALIGNED_1024 = 0x18000000,
    Q_ALIGNED_2048 = 0x1A000000,
    Q_ALIGNED_4096 = 0x1C000000,
    Q_ALIGNED_8192 = 0x1E000000,
    Q_ALIGNED_MASK = 0x1E000000
};

enum {
    EXPORT_FLAGS_INTERNAL = 0x10,
    EXPORT_FLAGS_IMPORT = 0x20,
    EXPORT_FLAGS_EXPORT = 0x40
};
#define FITS_IN_SHORT(value) ((value) == ((short)(value)))
#define FITS_IN_USHORT(value) ((value) == ((unsigned short)(value)))
#define FITS_IN_HI_SHORT(value) ((value) == (value & 0xFFFF0000))
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#define FITS_IN_SHORT2(value) (((short)(value)) == (value))
#define HIGH_PART(value) ((short)(((value) >> 16) + (((value) >> 15) & 1)))
#define LOW_PART(value) ((short)(value))

#define Q_INTERRUPT 0x80000000
#pragma pack(push, 2)
#else
#define FITS_IN_SHORT2(value) (((short)(value)) == (value))
#define HIGH_PART(value) ((short)(((value) >> 16) + (((value) >> 15) & 1)))
#define LOW_PART(value) ((short)(value))

#if MWCCPSP
#define Q_INTERRUPT 0 // no interrupt functions in 3.0.1 (CDecl_GetFunctionObject's qualifier mask)
#else
#define Q_INTERRUPT 0x80000000
#endif
#if !MWCCPSP // 3.0.1: natural alignment
#pragma pack(push, 2)
#endif
#endif
struct HashNameNode {
    struct HashNameNode* next;
    SInt32 id;
    SInt16 hashval;
    char name[2];
};
typedef struct ObjRegInfo {
    char rclass;
    char reg;
} ObjRegInfo;

typedef enum RegClass {
    RegClass_Invalid = -1,
    RegClass_SPR = 0,
    RegClass_CRFIELD = 1,
    RegClass_VR = 2,
    RegClass_FPR = 3,
    RegClass_GPR = 4,
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    RegClassMax = 5,
    RegClass_CRBIT = 6,
    RegClass_DCR = 7
} RegClass;

struct CInt64 {
#if MWCC_CORE == MWCC_CORE_B233
    // 2.3.3: 64-bit (CInt64_Or 0x4B6B60 takes two 8-byte values, returns edx:eax;
    // Object's value at +0x24 is followed by the link name at +0x2C)
    SInt32 hi;
    UInt32 lo;
#else
    // b69: 128-bit constants, as mwccps2 3.0.1 (word 0 most significant)
    SInt32 hi;
    UInt32 mid[2];
    UInt32 lo;
#endif
#else
    RegClassMax = 5,
    RegClass_CRBIT = 6,
    RegClass_DCR = 7
} RegClass;

struct CInt64 {
#if MWCCPS2
    // mwccps2 b210: 128-bit constants (R5900); word 0 is the most
    // significant, the low word is at +0xC (CInt64_ConvertInt32 tests its
    // sign there and fills words 0..2; CInt64_IsZero loops over 4 words)
    SInt32 hi;
    UInt32 mid[2];
    UInt32 lo;
#else
    SInt32 hi;
    UInt32 lo;
#endif
#endif
};
// functions 2.3.3 returns as int where GC 2.0 has Boolean (mov eax, 1 /
// xor eax, eax at their returns)
#if MWCC_CORE == MWCC_CORE_B233
#define BOOL_INT int
#else
#define BOOL_INT Boolean
#endif

#if MWCC_CORE == MWCC_CORE_B233 || (!MWCC_CORE && !MWCCPS2)
// a 64-bit CInt64 (2.3.3, 3.0.1, GC): the copied 3.0.1 back end's 128-bit
// words 1 and 2: with 64 bits, word 2 (bits 32..63) is hi; word 1 only ever
// pairs with hi in a test or store of the upper half
#define CI64_MID0 hi
#define CI64_MID1 hi
#else
#define CI64_MID0 mid[0]
#define CI64_MID1 mid[1]
#endif
union FloatStorage {
    int words[2];
    double value;
};
struct Float {
    FloatStorage data;
};
union MWVector128 {
    UInt8 uc[16];
    UInt16 us[8];
    UInt32 ul[4];
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    float f[4];
};
#pragma pack(pop)
#else
    float f[4];
};
#if !MWCCPSP // 3.0.1: natural alignment
#pragma pack(pop)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
