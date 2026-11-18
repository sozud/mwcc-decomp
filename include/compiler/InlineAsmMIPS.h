#include "compiler/mwcc_core.h"
#ifndef COMPILER_INLINEASMMIPS_H
#define COMPILER_INLINEASMMIPS_H

// mwccpsp 3.0.1: the MIPS inline assembler's statement records (an ST_ASM
// statement's expr)

#include "compiler/common.h"
#include "compiler/CInt64.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MOperandType {
    MOpnd_Imm = 0, // u.imm
    MOpnd_Reg = 1, // u.reg: a register, or the local variable in one
    MOpnd_GlobalObj = 2, // u.obj
    MOpnd_LocalObj = 3, // u.obj
    MOpnd_Lab = 4, // u.lab
    MOpnd_GCCReg = 5, // u.gcc: %N, a register operand of a GCC asm statement
    MOpnd_GCCImm = 6 // u.gcc: %N, its immediate operand
} MOperandType;

// an operand (0x10 bytes; mwccps2: 0x14)
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
// b69 packs these records to 2 bytes: CodeGen_GetAsmEffects (0x4C0B00) steps
// operands by 0x12 and reads a register's object at +6, its effect at +0xA,
// a memory operand's offset at +2 and object at +6, a label at +2; the
// opcode table (0x56E958) has 0x2C-byte entries with flags at +0x16
#pragma pack(push, 2)
#endif
typedef struct MAsmOperand {
    UInt8 type;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    UInt8 pad1[1];
#else
    UInt8 pad1[3];
#endif
#if MWCCPS2
    // mwccps2 b210: the immediate is a 16-byte CInt64 and a register's
    // effect/rclass overlap its third word (op++ steps by 0x14 in
    // InlineAsm_GetObjectOffset; func_00488D60 still reads effect at +0xC)
    union {
        union {
            CInt64 imm;
            struct {
                SInt32 num;
                struct Object* object;
            } reg;
            struct {
                SInt32 offset;
                struct Object* obj;
            } obj;
            struct {
                struct CLabel* label;
            } lab;
            struct {
                SInt32 num; // the GCC operand's index
                SInt16 effect;
                SInt16 rclass;
                char xyzw; // b210: a VU0 field suffix (x 1, y 2, z 4, w 8)
            } gcc;
        } u;
        struct {
#if MWCCPSP_NATIVE
            // natively reg.object is 8 bytes at +8: keep effect/rclass past it
            UInt8 ps2_pad04[16];
#else
            UInt8 ps2_pad04[8];
#endif
            SInt16 effect; // a register's: 1 read, 2 written
            SInt16 rclass;
            char xyzw; // b210: a register's VU0 field suffix
        };
    };
#else
    union {
        CInt64 imm;
        struct {
            SInt32 num;
            struct Object* object;
        } reg;
        struct {
            SInt32 offset;
            struct Object* obj;
        } obj;
        struct {
            struct CLabel* label;
        } lab;
        struct {
            SInt32 num; // the GCC operand's index
            SInt16 effect;
            SInt16 rclass;
        } gcc;
    } u;
    SInt16 effect; // a register's: 1 read, 2 written
    SInt16 rclass;
#endif
} MAsmOperand;

// an instruction: 8 bytes and its operands
typedef struct MAsmInstruction {
    SInt32 opcode;
    SInt16 flags; // 4 volatile, 8 ...
    SInt16 nops;
    MAsmOperand ops[1];
} MAsmInstruction;

typedef struct OpcodeDef {
    char* name;
    char* alias;
    char* format;
    UInt8 nargs;
    UInt8 size;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
    UInt8 pad0E[2];
#endif
    UInt32 unk10;
    UInt8 pad14[4];
    UInt32 flags;
    UInt32 flags2;
    char* intrinsic;
    SInt32 intrinsicid;
    SInt32 unk28;
    UInt16 unk2C;
} OpcodeDef; // 0x30
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#pragma pack(pop)
#if INLINEASM_OURS
// the source-only/native links' assembler (src/srconly/psp/*_ours.c) keeps
// b69's opcode numbers and indexes b69_asm_opcodes, the tree's entries in
// b69's numbering; its pcode gets the tree's number (ASM_BE_OP)
// (src/srconly/psp/OpcodeMap69.c, re/gen_opcode_map69.py)
#define data_006115F8 b69_asm_opcodes
extern void b69_asm_sync_opcodes(void);
extern UInt16 b69_tree_opcode(int op);
#define ASM_BE_OP(op) b69_tree_opcode(op)
extern struct OpcodeDef* b69_tree_def(int op);
#define ASM_TREE_DEF(op) b69_tree_def(op)
#else
#define ASM_BE_OP(op) (op)
#define ASM_TREE_DEF(op) (&data_006115F8[op])
#endif
#endif
extern OpcodeDef data_006115F8[]; // the opcodes (0x5C5)
extern UInt16 data_00625CE4[]; // an opcode's form with a register offset, from 0x518

// a name's meaning (InlineAsm_LookupSymbolMIPS)
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#pragma pack(push, 2) // b69: 0x22 bytes (gcc_name_list's stride at 0x56BFC0)
#endif
typedef struct MLookupResult {
    struct HashNameNode* name;
    struct CLabel* label;
    struct Object* object;
    struct Type* type;
    CInt64 value;
    Boolean has_value;
} MLookupResult;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
#pragma pack(pop)
#endif

extern Boolean InlineAsm_LookupSymbolMIPS(struct HashNameNode* name, MLookupResult* result);
extern CInt64 InlineAsm_Int64Expression(void);

#ifdef __cplusplus
}
#endif

#endif
