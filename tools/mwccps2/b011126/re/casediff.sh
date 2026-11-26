#!/bin/bash
# casediff.sh CASE_ID_PREFIX [LINES]: run one 011126 corpus case through this checkout's native and diff
# the disassembly of expected.o (the original's) against ours. Run from the repository root.
# The case is copied to a temporary directory, so the shared corpus is only read.
CORPUS=${CORPUS:-$HOME/Downloads/mweppc-b011126/build/mwccps2-b011126-corpus}
RUNNER=${RUNNER:-$HOME/Downloads/sotn-mwccps2-b198/tools/mwccpsp/corpus/run_corpus.py}
OD=${OD:-$HOME/Downloads/mipsel-binutils/bin/mipsel-linux-gnu-objdump}
id=$(ls "$CORPUS" | grep "^$1" | head -1)
W=$(mktemp -d)
mkdir -p "$W/c"
cp -R "$CORPUS/$id" "$W/c/$id"
echo "$id" > "$W/ids"
python3 "$RUNNER" --corpus "$W/c" --cc "$PWD/build/PS2_3_0_011126/native/x86_64/mwccps2" --cases "$W/ids" --keep -j1 | tail -1
for f in expected actual; do
    "$OD" -d -r -z -m mips:5900 "$W/c/$id/$f.o" 2>/dev/null | tail -n +4 | sed -E 's/^ *[0-9a-f]+:\t[0-9a-f]{8} \t?//' > "$W/$f.s"
done
echo "work dir: $W (expected.s, actual.s, c/$id/input.c, c/$id/args.json)"
diff "$W/expected.s" "$W/actual.s" | head -"${2:-60}"
