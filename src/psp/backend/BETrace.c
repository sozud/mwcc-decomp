// Debug: a comparable listing of every block's instructions, for diffing a
// traced hybrid (MWCCPSP_EXTRA_FLAGS=-DMWCCPSP_TRACE=1) against the native
// build (MWCCPSP_TRACE in the environment) after each back-end pass. Empty in
// the matching build, where TRACE_PCODE expands to nothing.
#include "compiler/common.h"
#include "psp/BE.h"
#define BE_TRACE_ALIAS 1
#include "compiler/Alias.h"

void BE_TracePCode(char* label) {
    MPCodeBlock* block;
    MPCode* pc;
    MOperand* op;
    int i;

    if (!getenv("MWCCPSP_TRACE")) {
        return;
    }
    fprintf(stderr, "== %s\n", label);
    for (block = data_00624FD0; block; block = block->next) {
        fprintf(stderr, "B%d:", (int)block->number);
        for (pc = block->first; pc; pc = pc->next) {
            fprintf(stderr, "\n  %x f%08x", pc->op, (unsigned)pc->flags);
#ifdef BE_TRACE_ALIAS
            if (pc->alias) {
                struct Alias* al = (struct Alias*)pc->alias;
                fprintf(
                    stderr,
                    " A%d:%d,%d,%d",
                    (int)al->type,
                    (int)al->offset,
                    (int)al->size,
                    (int)al->index
                );
            }
#endif
            for (i = 0, op = pc->args; i < pc->argcount; i++, op++) {
                switch (op->kind) {
                    case 0:
                        fprintf(stderr, " r%d:%d/%x", op->rclass, op->reg, op->flags);
                        break;
                    case 6:
                        fprintf(
                            stderr,
                            " L/%x:%d",
                            op->flags,
                            op->ptr ? (int)((MPCodeLabel*)op->ptr)->block->number : -1
                        );
                        break;
                    case 4:
                        fprintf(stderr, " m%d/%x", (int)op->value, op->flags);
                        break;
                    default:
                        fprintf(
                            stderr,
                            " k%d:%d,%d/%x",
                            op->kind,
                            (int)op->value,
                            (int)op->value2,
                            op->flags
                        );
                        break;
                }
            }
        }
        fprintf(stderr, "\n");
    }
}

#include "compiler/InlineAsmMIPS.h"
#include <stdio.h>
#include "compiler/CInt64.h"
#include "compiler/CParser.h"
// b69: a listing at each of b69's own BE_DumpPCode calls (#pragma
// debug_listing on), comparable between a trace hybrid, whose BE_DumpPCode is
// bound to b69's empty one at 0x4A5B90 (build_b69.py with
// MWCCPS2_B69_EXTRA_FLAGS=-DMWCCPS2_B69_TRACE=1 and BE_DumpPCode in
// MWCCPS2_B69_TRY), and the native build (MWCCPS2_DUMP in the environment),
// both on stderr.
// Mnemonics, not opcode numbers (b69 numbers its opcodes differently). The
// hybrid reads b69's operands raw: 0x16 bytes, kind, class, flags at +2 and
// register at +4, a label's pointer at +2.
#define OP_KIND(pc, i) ((pc)->args[i].kind)
#define OP_CLASS(pc, i) ((pc)->args[i].rclass)
#define OP_FLAGS(pc, i) ((pc)->args[i].flags)
#define OP_REG(pc, i) ((pc)->args[i].reg)
#define OP_LABEL(pc, i) ((MPCodeLabel*)(pc)->args[i].ptr)
#define OP_IMM(pc, i) \
    ((pc)->args[i].kind == 2 ? (UInt32)(pc)->args[i].value \
                             : CInt64_GetULong((CInt64*)&(pc)->args[i].value))
void BE_DumpPCode(char* name, char* label) {
    MPCodeBlock* block;
    MPCode* pc;
    int i;
    FILE* out = stderr;

    if (!getenv("MWCCPS2_DUMP")) {
        return;
    }
    if (label[0] == 'A' &&
        label[1] == 'f' &&
        label[6] == 'c' &&
        label[7] == 'o' &&
        label[8] == 'd') {
        // "After code selection": the options the passes test
        fprintf(
            out,
            "== %s options O%d cs%d li%d ph%d sch%d\n",
            name,
            copts.optimizationlevel,
            copts.commonsubs,
            copts.loopinvariants,
            copts.peephole,
            copts.schedule
        );
    }
    fprintf(out, "== %s %s\n", name, label);
    for (block = data_00624FD0; block; block = block->next) {
        fprintf(out, "B%d:", (int)block->number);
        for (pc = block->first; pc; pc = pc->next) {
            fprintf(out, "\n  %s f%08x", data_006115F8[pc->op].name, (unsigned)pc->flags);
            for (i = 0; i < pc->argcount; i++) {
                switch (OP_KIND(pc, i)) {
                    case 0:
                        fprintf(out, " r%d:%d/%x", OP_CLASS(pc, i), OP_REG(pc, i), OP_FLAGS(pc, i));
                        break;
                    case 6:
                        fprintf(
                            out, " L%d", OP_LABEL(pc, i) ? (int)OP_LABEL(pc, i)->block->number : -1
                        );
                        break;
                    case 2:
                    case 3:
                        fprintf(out, " k%d:%x", OP_KIND(pc, i), (unsigned)OP_IMM(pc, i));
                        break;
                    case 4:
                        fprintf(out, " m");
                        break;
                    default:
                        fprintf(out, " k%d", OP_KIND(pc, i));
                        break;
                }
            }
        }
        fprintf(out, "\n");
    }
    fflush(out);
}
