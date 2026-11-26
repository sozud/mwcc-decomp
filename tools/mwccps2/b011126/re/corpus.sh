#!/bin/bash
# corpus.sh [NAME] [run_corpus args...]: build this checkout's native 011126 and run the 011126 corpus
# through it -> build/run-NAME.txt (last line: N/6245 identical). Run from the repository root.
#   CORPUS (default ~/Downloads/mweppc-b011126/build/mwccps2-b011126-corpus, shared read-only)
#   RUNNER (default ~/Downloads/sotn-mwccps2-b198/tools/mwccpsp/corpus/run_corpus.py)
set -e
name=${1:-latest}; shift || true
CORPUS=${CORPUS:-$HOME/Downloads/mweppc-b011126/build/mwccps2-b011126-corpus}
RUNNER=${RUNNER:-$HOME/Downloads/sotn-mwccps2-b198/tools/mwccpsp/corpus/run_corpus.py}
python3 tools/mwccps2/b011126/native_b011126.py -j ${JOBS:-8} 2>&1 | grep -E 'units compile|error:|failed|undefined' | grep -v '^[0-9]* undefined' || true
python3 "$RUNNER" --corpus "$CORPUS" --cc "$PWD/build/PS2_3_0_011126/native/x86_64/mwccps2" -j ${JOBS:-8} "$@" > build/run-$name.txt 2>&1 || true
tail -1 build/run-$name.txt
