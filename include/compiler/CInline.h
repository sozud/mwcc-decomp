#include "compiler/mwcc_core.h"
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#ifndef COMPILER_CINLINE_H
#define COMPILER_CINLINE_H

#include "compiler/common.h"
#include "compiler/CFunc.h"
#include "compiler/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 2)

/* b69 0x459010 / 0x4591B0 (CMach_Get/SetBackEndOptions) copy these copts
   fields; 0x34 bytes (CInline_GenerateDeferredFuncs' 0x38 frame, CI_FuncData
   options at +0x12). +0x17 and +0x31 are never copied; fastmath is saved
   but not restored. */
struct CInlineBackendOptions {
    int unknown3D; // 0x00
    SInt16 mips_flags02; // 0x04
    UInt8 optimizationlevel; // 0x06
    UInt8 force_active; // 0x07
    UInt8 unsigned_char; // 0x08
    UInt8 interrupt; // 0x09
    UInt8 debuglisting; // 0x0A
    UInt8 dumpir; // 0x0B
    UInt8 optimizesize; // 0x0C
    UInt8 far_call; // 0x0D
    UInt8 inline_intrinsics; // 0x0E
    UInt8 schedule; // 0x0F
    UInt8 tailcall; // 0x10
    UInt8 unknown36; // 0x11
    UInt8 commonsubs; // 0x12
    UInt8 loopinvariants; // 0x13
    UInt8 propagation; // 0x14
    UInt8 deadstore; // 0x15
    UInt8 strengthreduction; // 0x16
    UInt8 unused17; // 0x17
    UInt8 lifetimes; // 0x18
    UInt8 unrolling; // 0x19
    UInt8 multiplepasses; // 0x1A
    UInt8 unrollfactor; // 0x1B
    UInt8 unrollinstrfactor; // 0x1C
    UInt8 peephole; // 0x1D
    UInt8 trunc; // 0x1E
    UInt8 fast_fptosi; // 0x1F
    UInt8 prefetch; // 0x20
    UInt8 sqrt; // 0x21
    UInt8 mips_option23; // 0x22
    UInt8 asmsemicolcomment; // 0x23
    UInt8 asmpoundcomment; // 0x24
    UInt8 unknown66; // 0x25
    UInt8 nofpregforblkmv; // 0x26
    UInt8 mips_option33; // 0x27
    UInt8 divbyzerocheck; // 0x28
    UInt8 mips_option3A; // 0x29
    UInt8 padloop; // 0x2A
    UInt8 cats; // 0x2B
    UInt8 nomacro; // 0x2C
    UInt8 dividefix; // 0x2D
    UInt8 usefloatacc; // 0x2E
    UInt8 warn_inlineasm; // 0x2F
    UInt8 irocseglobaladdresses; // 0x30
    UInt8 unused31; // 0x31
    UInt8 fastmath; // 0x32
};

struct CI_Var {
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    UInt8 sflags;
    SInt8 used;
    UInt8 b69_unk;
    SInt8 unmodified;
};

struct CI_SwitchCase {
    short labelID;
    CInt64 min;
    CInt64 max; // b69: 0x22-byte entries, max at +0x12 (CInline_UnpackSwitch)
};

struct CI_Switch {
    ENode* expr;
    Type* exprtype;
    short defaultlabelID;
    short numcases;
    CI_SwitchCase cases[1];
};

struct CI_Statement {
    StatementType type;
    UInt8 flags;
    UInt16 value;
    SInt32 sourceoffset;
// 2.3.3: none either (dispmap.py: dobjstack 0xC -> 0x8, u 0x10 -> 0xC)
#if MWCC_CORE != MWCC_CORE_B233
    HashNameNode* sourcefilepath;
#endif
    ExceptionAction* dobjstack;
    union {
        SInt16 statementnum;
        ENode* expr;
        struct {
            ENode* expr;
            SInt16 statementnum;
        } ifgoto;
        CI_Switch* switchdata;
        struct {
            void* data;
            SInt32 size;
        } asmdata;
    } u;
};

#if MWCC_CORE == MWCC_CORE_B52
// b52 has 3.0.1's one array, arguments then locals (statements at +0x0A:
// CInline_AddFRefList_InlineFunc)
#else
#define CI_FUNCDATA_ARGS(fd) ((fd)->arguments)
#define CI_FUNCDATA_LOCALS(fd) ((fd)->locals)
#define CI_FUNCDATA_NUMLOCALS(fd) ((fd)->numlocals)
#endif
struct CI_FuncData {
#if MWCC_CORE == MWCC_CORE_B52
    short numvars;
#endif
    short numarguments;
#if MWCC_CORE == MWCC_CORE_B52
    CI_Var* vars;
#else
    CI_Var* arguments;
    short numlocals;
    CI_Var* locals;
#endif
    short numstatements;
    CI_Statement* statements;
    struct CInlineBackendOptions options;
    FileOffsetInfo fileoffset;
    SInt32 symdecloffset;
    SInt32 functionbodyoffset;
    HashNameNode* functionbodypath;
    SInt32 symdeclend;
    UInt8 can_inline;
};
#if MWCC_CORE == MWCC_CORE_B52
#define CI_FUNCDATA_ARGS(fd) ((fd)->vars)
#define CI_FUNCDATA_LOCALS(fd) ((fd)->vars + (fd)->numarguments)
#define CI_FUNCDATA_NUMLOCALS(fd) ((fd)->numvars - (fd)->numarguments)
#endif

typedef enum CI_ActionType {
    CI_ActionInlineFunc = 0,
    CI_ActionMemberFunc = 1,
    CI_ActionTemplateFunc = 2,
    CI_ActionDefaultFunc = 3
} CI_ActionType;

struct CI_Action {
    struct CI_Action* next;
    Object* obj;
    union {
        struct {
            FileOffsetInfo fileoffset;
            TokenStream stream;
            TypeClass* tclass;
        } inlinefunc;
        struct {
            TemplClass* templ;
            TemplClassInst* inst;
            TemplateMember* tmemb;
        } memberfunc;
        struct {
            TemplateFunction* func;
            TemplFuncInstance* inst;
        } templatefunc;
    } u;
    CI_ActionType actiontype;
};

typedef enum CInlineCopyMode {
    CopyModeLocal,
    CopyModeGlobal,
    CopyModePack,
    CopyModeUnpack,
    CopyModeExpand
} CInlineCopyMode;

#pragma pack(pop)

extern CI_Action* cinline_tactionlist;

extern void CInline_Init(void);
extern SInt32 CInline_GetLocalID(Object* object);
extern Boolean CInline_ExpressionHasSideEffect(ENode* expr);
extern ENode* CInline_CopyExpression(ENode* expr, CInlineCopyMode mode);
extern void CInline_SerializeStatement(Statement* stmt);
extern Object* CInline_GetLocalObj(SInt32 id, Boolean inlineCopy);
extern SInt16 CInline_GetStatementNumber(Statement* first, Statement* stmt);
extern void CInline_PackIFunctionData(CI_FuncData* funcdata, Statement* stmt, Object* object);
extern void CInline_UnpackIFunctionData(
    Object* object, CI_FuncData* funcdata, Statement* firstStmt
);
extern void CInline_AddInlineFunctionAction(
    Object* object,
    TypeClass* tclass,
    FileOffsetInfo* fileoffset,
    TokenStream* stream,
    Boolean isTemplate
);
extern void CInline_AddTemplateFunctionAction(Object* object, void* func, void* inst);
extern void CInline_ObjectAddrRef(Object* object);
extern void CInline_GenFunc(Statement* stmt, Object* object, UInt8 oldStyleParameters);
extern Boolean CInline_CanFreeLHeap(void);
extern Boolean CInline_GenerateDeferredFuncs(void);
extern void CInline_Finish(void);

#ifdef __cplusplus
}
#endif

#endif
#else
#ifndef COMPILER_CINLINE_H
#define COMPILER_CINLINE_H

#include "compiler/common.h"
#include "compiler/CFunc.h"
#include "compiler/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !MWCCPSP // 3.0.1: natural alignment (CI_Action is 0x2C)
#pragma pack(push, 2)
#endif

#if MWCCPSP
// 3.0.1 (0x1E4 bytes, CMach_GetBackEndOptions 0x464D50): the first 0x144
// bytes of copts and volatileasm
struct CInlineBackendOptions {
#if MWCCPS2
    UInt8 copts[0x14C]; // b210: CMach_GetBackEndOptions (0x470220) saves 0x14C bytes
    UInt8 unk14C[0x94];
#else
    UInt8 copts[0x144];
    UInt8 unk144[0x9C];
#endif
    UInt8 volatileasm; // +0x1E0
    UInt8 pad1E1[3];
};
#else
struct CInlineBackendOptions {
    SInt16 processor;
    UInt8 scheduling;
    UInt8 schedule_factor;
    UInt8 debuglisting;
    UInt8 profile;
    UInt8 traceback;
    UInt8 disable_registers;
    UInt8 fp_contract;
    UInt8 no_register_save_helpers;
    UInt8 unroll_speculative;
    SInt16 unroll_instr_limit;
    SInt16 unroll_factor_limit;
    UInt8 altivec_model;
    UInt8 altivec_vrsave;
    UInt8 function_align;
    UInt8 cats;
    UInt8 force_cats;
    SInt32 sdata_threshold;
    SInt32 sdata2_threshold;
    UInt8 pool_data;
    UInt8 common;
    UInt8 use_fp;
    UInt8 soft_float;
    UInt8 codegen_pic;
    UInt8 full_dwarf;
    UInt8 no_comment_records;
    UInt8 rel109_offset;
    struct ObjGenSection* textSection;
    struct ObjGenSection* dataSection;
    struct ObjGenSection* constSection;
    struct ObjGenSection* sdataSection;
    struct ObjGenSection* sconstSection;
    SInt32 sectionHeaderTableSize;
    struct SectionSymbolAttributes** sectionHeaderTable;
    struct InterruptList* interruptList;
    struct InterruptGenerationRecord* interruptOptions;
    UInt8 incompatible_return_small_structs;
    UInt8 create_file_object;
    UInt8 incompatible_sfpe_double_params;
    UInt8 ibm_stackframe;
    UInt8 gen_isel;
    UInt8 gen_fsel;
    UInt8 gecko_float_typecons;
    UInt8 disable_vector_frame;
    UInt8 opt_bcc_lr_ctr;
    UInt8 use_lmw_stmw;
    UInt8 report_heap_info;
    UInt8 misaligned_mem_access;
    UInt8 switch_tables;
    UInt8 prepare_compress;
    UInt8 min_struct_alignment;
    UInt8 unsafe_global_reg_vars;
    UInt8 ignore_global_reg_vars;
    UInt32 bc_range;
    UInt8 gprfloatcopy;
#if VERSION >= VERSION_GC_2_5
    UInt8 old_style_bitfields;
#endif
    UInt8 optimizationlevel;
    UInt8 optimizesize;
    UInt8 optimizewithasm;
    UInt8 commonsubs;
    UInt8 loopinvariants;
    UInt8 propagation;
    UInt8 deadstore;
    UInt8 strengthreduction;
    UInt8 strengthreductionstrict;
    UInt8 deadcode;
    UInt8 lifetimes;
    UInt8 unrolling;
    UInt8 vectorizeloops;
    UInt8 multiplepasses;
    UInt8 opt_pointer_analysis;
    UInt8 opt_pointer_analysis_mode;
    UInt8 unrollfactor;
    UInt8 unrollinstrfactor;
};
#endif

#if MWCCPSP
// 3.0.1 (0x14 bytes): the object's uid identifies it in packed expressions;
// arguments have sflags 0x40
struct CI_Var {
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    SInt32 uid;
    UInt8 sflags;
    SInt8 used;
#if MWCCPS2
    UInt8 unk12; // b210: unmodified is at +0x13 (CInline_PackActions)
#endif
    SInt8 unmodified;
};
#else
struct CI_Var {
    HashNameNode* name;
    Type* type;
    UInt32 qual;
    UInt8 sflags;
    SInt8 used;
    SInt8 unmodified;
};
#endif

#if MWCCPS2
// b210 (CInline_PackSwitch 0x509A00): case ranges, then the template-dependent
// cases (SwitchInfo.depcases) with their bound expressions
struct CI_SwitchCase {
    short labelID;
    union {
        struct {
            CInt64 min;
            CInt64 max;
        } c;
        struct {
            ENode* min;
            ENode* max;
        } dep;
    } u;
};

struct CI_Switch {
    ENode* expr;
    Type* exprtype;
    short defaultlabelID;
    short numcases;
    short numdepcases;
    CI_SwitchCase cases[1];
};
#else
struct CI_SwitchCase {
    short labelID;
    CInt64 min;
};

struct CI_Switch {
    ENode* expr;
    Type* exprtype;
    short defaultlabelID;
    short numcases;
    CI_SwitchCase cases[1];
};
#endif

struct CI_Statement {
    StatementType type;
    UInt8 flags;
    UInt16 value;
    SInt32 sourceoffset;
    HashNameNode* sourcefilepath;
    ExceptionAction* dobjstack;
    union {
        SInt16 statementnum;
        ENode* expr;
        struct {
            ENode* expr;
            SInt16 statementnum;
        } ifgoto;
        CI_Switch* switchdata;
        struct {
            void* data;
            SInt32 size;
        } asmdata;
    } u;
};

#if MWCCPSP
// 3.0.1 (0x218 bytes, CInline_PackIFunctionData): one variable array,
// arguments first
struct CI_FuncData {
    short numvars;
    short numarguments;
    CI_Var* vars;
    short numstatements;
    CI_Statement* statements;
    struct CInlineBackendOptions options;
    FileOffsetInfo fileoffset;
    SInt32 symdecloffset;
    SInt32 functionbodyoffset;
    HashNameNode* functionbodypath;
    SInt32 symdeclend;
    UInt8 can_inline;
};
#define CI_FUNCDATA_ARGS(fd) ((fd)->vars)
#define CI_FUNCDATA_LOCALS(fd) ((fd)->vars + (fd)->numarguments)
#define CI_FUNCDATA_NUMLOCALS(fd) ((fd)->numvars - (fd)->numarguments)
#else
struct CI_FuncData {
    short numarguments;
    CI_Var* arguments;
    short numlocals;
    CI_Var* locals;
    short numstatements;
    CI_Statement* statements;
    struct CInlineBackendOptions options;
    FileOffsetInfo fileoffset;
    SInt32 symdecloffset;
    SInt32 functionbodyoffset;
    HashNameNode* functionbodypath;
    SInt32 symdeclend;
    UInt8 can_inline;
};
#endif

typedef enum CI_ActionType {
    CI_ActionInlineFunc = 0,
    CI_ActionMemberFunc = 1,
    CI_ActionTemplateFunc = 2,
    CI_ActionDefaultFunc = 3
} CI_ActionType;

struct CI_Action {
    struct CI_Action* next;
    Object* obj;
#if MWCCPSP
    void* unk8; // 3.0.1, not identified (the union is at +0xC)
#endif
    union {
        struct {
            FileOffsetInfo fileoffset;
            TokenStream stream;
            TypeClass* tclass;
        } inlinefunc;
        struct {
            TemplClass* templ;
            TemplClassInst* inst;
            TemplateMember* tmemb;
        } memberfunc;
        struct {
            TemplateFunction* func;
            TemplFuncInstance* inst;
        } templatefunc;
    } u;
    CI_ActionType actiontype;
};

typedef enum CInlineCopyMode {
    CopyModeLocal,
    CopyModeGlobal,
    CopyModePack,
    CopyModeUnpack,
    CopyModeExpand
} CInlineCopyMode;

#if !MWCCPSP
#pragma pack(pop)
#endif

extern CI_Action* cinline_tactionlist;

extern void CInline_Init(void);
extern SInt32 CInline_GetLocalID(Object* object);
extern Boolean CInline_ExpressionHasSideEffect(ENode* expr);
extern ENode* CInline_CopyExpression(ENode* expr, CInlineCopyMode mode);
extern void CInline_SerializeStatement(Statement* stmt);
extern Object* CInline_GetLocalObj(SInt32 id, Boolean inlineCopy);
extern SInt16 CInline_GetStatementNumber(Statement* first, Statement* stmt);
extern void CInline_PackIFunctionData(CI_FuncData* funcdata, Statement* stmt, Object* object);
extern void CInline_UnpackIFunctionData(
    Object* object, CI_FuncData* funcdata, Statement* firstStmt
);
extern void CInline_AddInlineFunctionAction(
    Object* object,
    TypeClass* tclass,
    FileOffsetInfo* fileoffset,
    TokenStream* stream,
    Boolean isTemplate
);
extern void CInline_AddTemplateFunctionAction(Object* object, void* func, void* inst);
extern void CInline_ObjectAddrRef(Object* object);
extern void CInline_GenFunc(Statement* stmt, Object* object, UInt8 oldStyleParameters);
extern Boolean CInline_CanFreeLHeap(void);
extern Boolean CInline_GenerateDeferredFuncs(void);
extern void CInline_Finish(void);

#ifdef __cplusplus
}
#endif

#endif
#endif
