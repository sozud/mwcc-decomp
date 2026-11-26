# mwccps2 3.0 011126 (Persona 3 FES's game code)

decomp.me `mwcps2-3.0-011126` (`get_orig.py`: the image's layer by digest). Persona 3 FES builds its
game code and `librtanim`/`librtquat` with it (`-O2 -gccincludes`, `-O3`).

011126 is b69's code with its own differences: 4,293 of b69's 4,494 functions are byte for byte in it
(relocations and call operands masked), the other 201 changed. The native is b69's sources with
`-DMWCCPS2_B69=1 -DMWCCPS2_B011126=1` (`MWCC_CORE_B011126`, include/compiler/mwcc_core.h), built by
`native_b011126.py` through `../native_core.py`.

| File | What |
| ---- | ---- |
| `match69.csv` | b69's functions and where each is in 011126 (`re/match69.py`) |
| `vamap.json` | b69 → 011126 addresses: matched functions, call targets, pointers in matched code, the `.data` alignment (`re/vamap.py`) |
| `symbols*.txt`, `functions.csv`, `glue/`, `units.txt` | b69's, every address translated (`re/gen_from_b69.py`); `dropped.txt` lists what has no 011126 address |
| `unmatched69.txt`, `unmatched_gaps.txt`, `compare.txt` | the 201 changed functions: size change, and how far 011126's code is from b69's and from 2.3.3's (`re/fdiff.py --list`, `--compare`) |
| `capture.py` | the corpus: every case of the b198 corpus (every PS2 corpus's inputs) replayed through the original under wibo |

Tools in `re/`: `fdiff.py ADDR [--noreg]` diffs one b69 function's x86 code against 011126's;
`optable.py` decodes and diffs the opcode table initializer; `copts_table.py` diffs the pragma tables.

## Differences ported

Every changed function in `worklist.txt` has been compared with 011126's code; the ports are in the
shared sources under `MWCC_CORE == MWCC_CORE_B011126` with the 011126 address in a comment. About
half the changed functions differ only in assert line numbers, string addresses, struct offsets
(copts is 4 bytes smaller, File lost a field) or register allocation, and need nothing. The ports,
by area:

- Options: `CodeGen_InitBackEndOptions` (padloop/dividefix only when not optimizing for size, no
  fastmath, VU0 registers off); the pragma table (no `fullpath_file`, `stdc_libfuncname_redefined`,
  `no-ansi-lib-builtin`, `warn_uninitializedvar`); `CMach_Get/SetBackEndOptions` keep the VU0
  settings; `#pragma vu0_register_number` (the b69 line had only an aborting stub) and the
  illegal-pragma warning's number.
- Front end: `__FILE__` (no escaping), one attribute per `__attribute__`, `#` comments and string
  escapes in assembler, `InlineAsm_SyntaxError`.
- Expressions and instruction selection: every assignment's target is its address after ExpandIR
  (2.3.3's form) and selection generates it whole; casts load their operand; compare-and-branch,
  multiply (constants, then the inline long long multiply, then `__muldi3`) and divide; no `sqrtf`
  intrinsic; `call_function`'s struct and kind-6 arguments.
- Back end: 011126's alias helper (0x439A80) in the load/store helpers, AliasOperand.c, PCodeInfo and
  mips.c; BE_macro's far-address lui alias; LocalCSE without 128-bit copies; peephole patterns;
  VReg kind 6; spill lqc2/sqc2; ObjGen relocations; the opcode table's VU flags.
- Scheduling and nops: `b69_writeslive` (live-out only), no padloop call in the delay-slot passes, the
  second nop only for a two-instruction loop, `ps2_padloop_4C2960` (2.3.3's search, pads to 8).
- Code generation: quadword-class returns, locals in VU0 registers (the 0x57AB9C list), the asm
  statement case of `CodeGen_Generator`, `IRO_CheckInit` always, `CMach_InitIntMem`, DWARF statics.
- Inline assembly: the MIPS assembler's operand parsing and errors, GCC-style asm
  (`gcc_parse_name`, `gcc_replace_arg_st_asm`, `gcc_copy_expression`, `gcc_parse_attribute`),
  `ScanStatements`, `ps2_IsQuadwordClass` without base classes.
- Driver: `SupportedPlugin`'s object flags (and the ELF importer's), older assert lines.
- VU0 registers (`#pragma enable_vu0_registers on` or a `j` asm operand, when optimizing):
  quadword-class temporaries and pointer temporaries live in VU0 registers (operand kind 6, the list
  at 0x57AB9C, `b011126_QuadLocalReg` 0x499360), flagged 128-bit integers in theirs (kind 5):
  `func_004949B0` (0x4A246E), `func_004A72B0` (0x499430), `func_004A64C0` (0x499770), the store
  (0x499E10), the struct copy `func_00496CB0` (0x4A0070), the store helper (0x48E294), the qmtc2
  peephole (0x507BE0); the front end copies a call result to a temporary as `func_00436B30` says
  (0x459100); 011126's MIPS error numbers (0x4BAB40). `tests/vu0/run.sh` compiles each test with
  both compilers at -O2 and -O4,p (no corpus case enables VU0 registers).


## Corpus

6,245 cases (`capture.py`; 1,254 rejected by 011126, mostly Persona 4 options it doesn't have; 1
nondeterministic). Native b69 as it was: 2,965. Native 011126: **6,067** (Persona 3 FES 110/111).

Every one of the other 178 is b69's arena residue (`corpus-ub.txt`): a constant's ENode `hascall`
byte is whatever an earlier allocation left, and `call_function` evaluates an argument with it set
first, so float constants are set up in another order or kept in a saved register. `re/hascall0.py`
patches a copy of the original to write 0 there, as the native does, and that patched original
compiles all 178 to exactly the native's object.

`re/classify.py` sorts failing cases by whether the differing function also differs when compiled
alone (it can't tell residue from earlier in the same function from a code difference; the patched
original can).

## Known gaps (the b69 line's native, not 011126's changes)

Every b69-line native resolves these to aborting glue stubs (`glue/code.txt`). Confirmed to abort:
`#pragma align`, `processor`, `section`, `define_section`, `maxtempforblkmv`, `vu0_max_used_nvreg`.
Also stubs, not reached by the tests tried: `CodeGen_ParseDeclSpec`, `CodeGen_ExpandSpecialMacro`,
`CMach_ParseGlobalRegisterVariable`, `ps2_lltod`/`ps2_lltof`/`ps2_ulltod` (long long to
float/double conversions, constant or not, compile byte-identical at -O0, -O2 and -O4,p). `-help` prints
nothing, `-version` exits 1, and the banner is 2.3.3's (the generated parser tables' offsets and the
missing VERSIONINFO).
