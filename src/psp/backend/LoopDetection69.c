// b69's loop detection on the MIPS back end's pcode (0x4AF430..0x4AFDE0):
// dominators, loop headers (a predecessor the block dominates), natural
// loops, the loop tree, preheaders, and with `blocks` a root holding every
// block plus a LOOP301_BLOCK leaf for each block not in an inner loop. It is
// 3.0.1's LoopDetection301.c one generation earlier: the loop records are
// 0x4C bytes (no 0x24-byte gap before the preheader), each block set is a
// {count, list, vector} triple that addblock keeps (adding a block only once),
// and the loop maker is a function of its own (0x4AF920). The callers are
// the code generator's padloop pass (0x43651A) and loop invariants
// (0x4AC070). The register pressure pass is at 0x509ED0..0x50A100 in b69,
// after the liveness code; it is at the end of this file.
#include "compiler/common.h"
#include "compiler/BitVectors.h"
#include "compiler/CParser.h"
#include "compiler/CompilerTools.h"
#include "psp/BE.h"
#include "psp/Loop301.h"

extern SInt32 data_00624D40; // pcblockcount
extern MPCodeBlock** data_00625418; // depthfirstordering
extern void computedepthfirstordering(void);
extern SInt32 func_00462160(char rclass);
extern void func_004C3460(MPCodeBlock* block);
extern MPCodeBlock* func_0046F660(MPCodeBlock* from, MPCodeBlock* to);
extern void func_004AFFC0(); // listing hook (empty)

// compiler/optimizer/LoopDetection.c has statics of these names, and the
// hybrid link (config/replace.txt, patch.link) goes by base names
#define computedominators computedominators69
#define findloopheaders findloopheaders69
#define findnaturalloop findnaturalloop69
#define addlooptolist addlooptolist69

// Loop301's {nblocks, blocks, membervec} and {nexits, exits, exitvec}
typedef struct BlockSet {
    SInt32 count;
    BlockList301* list;
    UInt32* vec;
} BlockSet;
#define BLOCKS(loop) ((BlockSet*)&(loop)->nblocks)
#define EXITS(loop) ((BlockSet*)&(loop)->nexits)

Loop301* data_006258BC; // the loop forest
SInt32 data_00624BA8; // loops numbered so far
RegPressure301 data_00624E04[7];

// .bss, last declared first (0x565B78..0x565B8B)
static UInt32** dominators;
static BlockList301* loopheaders;
static SInt32 nloopheaders;
static MPCodeBlock** loopstack; // findnaturalloop's stack
static SInt32 setbits; // bits in a block set (3 * pcblockcount)

// 0x4AF430
static void computedominators(void) {
    SInt32 n;
    int changed;
    SInt32 i;
    MPCodeBlock* block;
    MPBlockLink* link;
    UInt32* vec;

    n = data_00624D40;
    changed = 1;
    dominators = oalloc(data_00624D40 * sizeof(UInt32*));
    for (i = 0; i < data_00624D40; i++) {
        dominators[i] = oalloc(4 * ((n + 31) >> 5));
    }
    vec = oalloc(4 * ((n + 31) >> 5));
    bitvectorinitialize(dominators[data_00624FD0->number], n, 0);
    dominators[data_00624FD0->number][0] |= 1;
    for (block = data_00624FD0->next; block; block = block->next) {
        bitvectorinitialize(dominators[block->number], n, 0xFFFFFFFF);
    }
    computedepthfirstordering();
    while (changed) {
        for (changed = 0, i = 0; i < data_00624D40; i++) {
            block = data_00625418[i];
            if (block && block->number != data_00624FD0->number) {
                link = block->predecessors;
                bitvectorcopy(vec, dominators[link->from->number], n);
                for (link = link->nextpred; link; link = link->nextpred) {
                    bitvectorintersect(vec, dominators[link->from->number], n);
                }
                bitvectorsetbit(block->number, vec);
                if (bitvectorchanged(dominators[block->number], vec, n)) {
                    changed = 1;
                }
            }
        }
    }
}

// 0x4AF5E0
static BlockList301* findloopheaders(void) {
    MPCodeBlock* block;
    MPCodeBlock* pred;
    MPBlockLink* link;
    BlockList301* bl;

    loopheaders = NULL, nloopheaders = 0;
    for (block = data_00624FD0->next; block; block = block->next) {
        for (link = block->predecessors; link && (pred = link->from, 1); link = link->nextpred) {
            if (bitvectorgetbit(block->number, dominators[pred->number])) {
                bl = oalloc(sizeof(BlockList301));
                bl->block = block;
                bl->next = loopheaders;
                loopheaders = bl;
                nloopheaders++;
                break;
            }
        }
    }
    return loopheaders;
}

// 0x4AF670
static void initblockset(BlockSet* set) {
    set->count = 0;
    set->list = NULL;
    bitvectorinitialize(set->vec = lalloc(4 * ((setbits + 31) >> 5)), setbits, 0);
}

// 0x4AF6B0
static void addblock(BlockSet* set, MPCodeBlock* block) {
    BlockList301* bl;

    if (!bitvectorgetbit(block->number, set->vec)) {
        set->count++;
        bl = lalloc(sizeof(BlockList301));
        bl->block = block;
        bl->next = set->list, set->list = bl;
        bitvectorsetbit(block->number, set->vec);
    }
}

// 0x4AF710: block has a successor outside the loop
static Boolean isexit(Loop301* loop, MPCodeBlock* block) {
    MPBlockLink* link;
    MPCodeBlock* succ;

    for (link = block->successors; link && (succ = link->block, 1); link = link->next) {
        if (!bitvectorgetbit(succ->number, loop->membervec)) {
            return 1;
        }
    }
    return 0;
}

// 0x4AF760
static void findnaturalloop(Loop301* loop) {
    int sp;
    MPCodeBlock* header;
    MPCodeBlock* block;
    MPBlockLink* link;
    BlockList301* bl;

    sp = 0;
    addblock(BLOCKS(loop), loop->header);
    for (link = loop->header->predecessors; link && (block = link->from, 1);
         link = link->nextpred) {
        header = loop->header;
        if (bitvectorgetbit(header->number, dominators[block->number]) && block != header) {
            addblock(BLOCKS(loop), block);
            loopstack[sp++] = block;
        }
    }
    while (sp) {
        for (link = loopstack[--sp]->predecessors; link && (block = link->from, 1);
             link = link->nextpred) {
            if (!bitvectorgetbit(block->number, loop->membervec)) {
                addblock(BLOCKS(loop), block);
                loopstack[sp++] = block;
            }
        }
    }
    for (bl = loop->blocks; bl; bl = bl->next) {
        if (isexit(loop, bl->block)) {
            addblock(EXITS(loop), bl->block);
        }
    }
    if (loop->nexits == 0) {
        loop->flags |= 1;
    }
}

// 0x4AF890
static void addlooptolist(Loop301* loop, Loop301** list) {
    Loop301** scan;
    Loop301* l;

    scan = list;
    while ((l = *scan)) {
        if (bitvectorgetbit(loop->header->number, l->membervec)) {
            loop->parent = l;
            addlooptolist(loop, &l->children);
            return;
        }
        if (bitvectorgetbit(l->header->number, loop->membervec)) {
            *scan = l->next;
            l->parent = loop;
            l->next = loop->children, loop->children = l;
        } else {
            scan = &l->next;
        }
    }
    loop->next = *list, *list = loop;
}

// a new loop record (inline in each maker)
#define NEWLOOP(loop, k) \
    (loop) = lalloc(sizeof(Loop301)); \
    memclrw((loop), sizeof(Loop301)); \
    (loop)->kind = (k); \
    (loop)->id = data_00624BA8++

// 0x4AF920
static void findloops(void) {
    Loop301* loop;

    loopstack = oalloc(data_00624D40 * sizeof(MPCodeBlock*));
    for (; loopheaders; loopheaders = loopheaders->next) {
        NEWLOOP(loop, LOOP301_LOOP);
        loop->header = loopheaders->block;
        initblockset(BLOCKS(loop));
        initblockset(EXITS(loop));
        findnaturalloop(loop);
        addlooptolist(loop, &data_006258BC);
    }
}

// 0x4AF9C0: the loop's preheader, made when its single outside predecessor
// falls through into the header.
static void insertpreheader(Loop301* loop) {
    MPCodeBlock* pre;
    MPCodeBlock* header;
    MPCodeBlock* block;
    MPBlockLink* link;
    MPBlockLink* succ;
    Loop301* p;

    pre = NULL;
    header = loop->header;
    for (link = header->predecessors; link && (block = link->from, 1); link = link->nextpred) {
        if (!bitvectorgetbit(block->number, loop->membervec)) {
            if (pre) {
                return;
            }
            pre = block;
        }
    }
    if (pre) {
        succ = pre->successors;
        if (!((succ && !succ->next) ||
              (succ && succ->next && !succ->next->next && pre->next == header))) {
            return;
        }
        if (pre->next == header) {
            pre = func_0046F660(pre, header);
            for (p = loop->parent; p; p = p->parent) {
                addblock(BLOCKS(p), pre);
            }
        }
    }
    loop->preheader = pre;
    if (!loop->children) {
        loop->flags |= 2;
    }
}

// 0x4AFA90: the root, holding every block
static void makeroot(void) {
    Loop301* root;
    Loop301* child;
    MPCodeBlock* block;

    NEWLOOP(root, LOOP301_ROOT);
    root->children = data_006258BC;
    data_006258BC = root;
    for (child = root->children; child; child = child->next) {
        child->parent = root;
    }
    initblockset(BLOCKS(root));
    for (block = data_00624FD0; block; block = block->next) {
        addblock(BLOCKS(root), block);
    }
}

// 0x4AFB10: a LOOP301_BLOCK leaf for each block not in an inner loop
static void addblockleaves(Loop301* loop) {
    UInt32* vec;
    BlockList301* bl;
    Loop301* l;
    Loop301** tail;

    bitvectorinitialize(vec = oalloc(4 * ((data_00624D40 + 31) >> 5)), data_00624D40, 0);
    for (bl = loop->blocks; bl; bl = bl->next) {
        bitvectorsetbit(bl->block->number, vec);
    }
    for (l = loop->children; l; l = l->next) {
        for (bl = l->blocks; bl; bl = bl->next) {
            bitvectorclearbit(bl->block->number, vec);
        }
    }
    for (tail = &loop->children; (l = *tail); tail = &l->next) {}
    for (bl = loop->blocks; bl; bl = bl->next) {
        if (bitvectorgetbit(bl->block->number, vec)) {
            NEWLOOP(l, LOOP301_BLOCK);
            l->header = bl->block;
            l->parent = loop;
            *tail = l;
            tail = &l->next;
        }
    }
}

// 0x4AFC20
static void walkloop(Loop301* loop, void (*func)(Loop301*), Boolean preorder) {
    Loop301* child;

    switch (loop->kind) {
        case LOOP301_LOOP:
        case LOOP301_ROOT:
            if (preorder) {
                func(loop);
            }
            for (child = loop->children; child; child = child->next) {
                walkloop(child, func, preorder);
            }
            if (!preorder) {
                func(loop);
            }
            break;
        case LOOP301_BLOCK:
            func(loop);
            break;
    }
}

// 0x4AFC80
void func_0054AD10(void (*func)(Loop301*)) {
    Loop301* loop;

    for (loop = data_006258BC; loop; loop = loop->next) {
        walkloop(loop, func, 0);
    }
}

// 0x4AFCB0: findloopsinflowgraph
Loop301* func_0054AB90(Boolean blocks) {
    data_006258BC = NULL;
    data_00624BA8 = 0;
    setbits = data_00624D40 * 3;
    computedominators();
    if (findloopheaders()) {
        findloops();
        if (copts.debuglisting) {
            func_004AFFC0();
        }
        func_0054AD10(insertpreheader);
    }
    if (blocks) {
        makeroot();
        func_0054AD10(addblockleaves);
    }
    freeoheap();
    return data_006258BC;
}

// the register pressure (b69 keeps it at 0x509ED0..0x50A100, after the liveness code)

// 0x509F20: steps the live sets back over pc
static void stepback(MPCode* pc) {
    MOperand* op;
    int n;

    for (op = pc->args, n = pc->argcount; n--; op++) {
        if (op->kind == 0 && (op->flags & 2)) {
            if (bitvectorgetbit(op->reg, data_0062541C[op->rclass].current)) {
                data_00624E04[op->rclass].count--;
            }
            bitvectorclearbit(op->reg, data_0062541C[op->rclass].current);
        }
    }
    for (op = pc->args, n = pc->argcount; n--; op++) {
        if (op->kind == 0 && (op->flags & 1)) {
            if (!bitvectorgetbit(op->reg, data_0062541C[op->rclass].current)) {
                data_00624E04[op->rclass].count++;
            }
            bitvectorsetbit(op->reg, data_0062541C[op->rclass].current);
        }
    }
}

// 0x509ED0: the live sets and counts at the end of block
static void startblock(MPCodeBlock* block) {
    char rclass;

    func_004C3460(block);
    for (rclass = 0; rclass < 7; rclass++) {
        data_00624E04[rclass].count =
            bitvectorcount(data_0062541C[rclass].current, data_0062541C[rclass].n);
    }
}

// 0x50A020: the register pressure of every block
static void computepressure(void) {
    char rclass;
    MPCodeBlock* block;
    MPCode* pc;

    for (rclass = 0; rclass < 7; rclass++) {
        data_00624E04[rclass].perblock = oalloc(data_00624D40 * sizeof(SInt32));
        data_00624E04[rclass].avail = func_00462160(rclass);
    }
    for (block = data_00624FD0; block; block = block->next) {
        startblock(block);
        for (rclass = 0; rclass < 7; rclass++) {
            data_00624E04[rclass].perblock[block->number] = data_00624E04[rclass].count;
        }
        for (pc = block->last; pc; pc = pc->prev) {
            stepback(pc);
            for (rclass = 0; rclass < 7; rclass++) {
                if ((UInt32)data_00624E04[rclass].perblock[block->number] <
                    (UInt32)data_00624E04[rclass].count) {
                    data_00624E04[rclass].perblock[block->number] = data_00624E04[rclass].count;
                }
            }
        }
    }
}

// 0x4AFD20
static void looppressure(Loop301* loop) {
    char rclass;
    BlockList301* bl;
    SInt32 v;

    switch (loop->kind) {
        case LOOP301_BLOCK:
            for (rclass = 0; rclass < 7; rclass++) {
                loop->pressure[rclass] = data_00624E04[rclass].perblock[loop->header->number];
            }
            break;
        case LOOP301_LOOP:
        case LOOP301_ROOT:
            for (rclass = 0; rclass < 7; rclass++) {
                loop->pressure[rclass] = 0;
            }
            for (bl = loop->blocks; bl; bl = bl->next) {
                for (rclass = 0; rclass < 7; rclass++) {
                    v = data_00624E04[rclass].perblock[bl->block->number];
                    if ((UInt32)v > (UInt32)loop->pressure[rclass]) {
                        loop->pressure[rclass] = v;
                    }
                }
            }
            break;
    }
}

// 0x4AFDD0
void func_0054AAB0(void) {
    computepressure();
    func_0054AD10(looppressure);
}
