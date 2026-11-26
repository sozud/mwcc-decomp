# 2.3.3 hascall residue: how much of native's failure it explains

2.3.3's call_function (0x49E830) tests ENode hascall bytes that
primary_expression never writes (the b69 problem). hcrec.py records every
test in the original (sites 0x49E925, 0x49E94F, 0x49E97B, 0x49EFFB, 0x49F06C)
and the native replays them through MWCCPS2_B233_HASCALL (FunctionCalls.c,
HASCALL(); native only). replay.py CASES NATIVE OUT runs this per case.

Measured 2026-11-15: native 454/1637. On a sample of 120 failing cases,
0 pass with the original's hascall values replayed, and every sequence stays
aligned. So 2.3.3's native failures are code differences (only about 24% of
functions are exact), not hascall residue. The residue still matters once
the code matches; b69 lost 11 of 1084 cases to it.

Re-measured 2026-11-17 (native 1540/1637, now 1541, the hook now in b233's own
FunctionCalls.c; the earlier probe had gone into the b69 copy, so it replayed
nothing): of the 97 native failures, 84 give expected.o with the original's
hascall bytes replayed, records in step. They are listed with the evidence in
tools/mwccps2/config/b233/corpus-ub.txt. MWCCPS2_B233_HASCALL_DIFF=1 writes
SEQ.diff: replayed values that differ from the native's own, on constant
nodes and on other nodes, then per ENode type (native 0 -> original 1, and
the reverse). The other nodes seen are operators over constants, whose byte
expandIR ORs from their operands. The rest (13, 12 since CExpr_AllBitsSet)
were taken further later the same day: 9 matched by porting 2.3.3's code, 2
more (10a13bb34be4, dc7d31fbcaa4) then pass with the hascall replay, and
9cff7ee317ec with the hascall replay plus the float-constant residue
(MWCCPS2_B233_FLOATRES, corpus-ub.txt). Native 1550/1637; the 87 failures
are all in corpus-ub.txt.
