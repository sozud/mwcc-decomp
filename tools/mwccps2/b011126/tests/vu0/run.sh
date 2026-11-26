#!/bin/bash
# run.sh [test.c ...]: compile each VU0 test with the original 011126 (under wibo) and the
# native, at -O2 and -O4,p, and report identical/differs (and crashes/aborts).
# A rejected input is identical when both print the same messages (tool name aside).
# Run from anywhere; uses the repository's orig/, build/tools/wibo and native build.
#   OBJDUMP (default ~/Downloads/mipsel-binutils/bin/mipsel-linux-gnu-objdump) for KEEP=1 diffs
cd "$(dirname "$0")"
root=$(cd ../../../../.. && pwd)
orig="$root/orig/PS2_3_0_011126/mwccps2.exe"
wibo="$root/build/tools/wibo"
native=${NATIVE:-$root/build/PS2_3_0_011126/native/x86_64/mwccps2}
OBJDUMP=${OBJDUMP:-$HOME/Downloads/mipsel-binutils/bin/mipsel-linux-gnu-objdump}
out=${OUT:-$root/build/vu0tests}
mkdir -p "$out"
[ $# -eq 0 ] && set -- t*.c t*.cpp
same=0; total=0
exec 3>&2 2>/dev/null # the shell's own "Segmentation fault" lines
for src in "$@"; do
  for opt in -O2 -O4,p; do
    t=$(basename "${src%.*}")$(echo $opt | tr -d ,)
    total=$((total+1))
    MWCIncludes=. nice -n 19 "$wibo" "$orig" -c $opt "$src" -o "$out/$t.orig.o" > "$out/$t.orig.log" 2>&1; ro=$?
    MWCIncludes=. nice -n 19 "$native" -c $opt "$src" -o "$out/$t.nat.o" > "$out/$t.nat.log" 2>&1; rn=$?
    if [ $ro -ne $rn ]; then
      echo "differs   $t (exit orig=$ro native=$rn: $(head -c 120 "$out/$t.nat.log" | tr '\n' ' '))"
    elif [ $ro -ne 0 ]; then
      if cmp -s <(tr -d "\r" < "$out/$t.orig.log" | sed "s/^### .* Compiler:/### Compiler:/") <(tr -d "\r" < "$out/$t.nat.log" | sed "s/^### .* Compiler:/### Compiler:/"); then
        if [ $ro -ge 128 ]; then
          echo "identical $t (both crash, signal $((ro - 128)))"
        else
          echo "identical $t (both rejected)"
        fi
        same=$((same+1))
      else
        echo "differs   $t (both rejected, messages differ)"
      fi
    elif cmp -s "$out/$t.orig.o" "$out/$t.nat.o"; then
      echo "identical $t"; same=$((same+1))
    else
      echo "differs   $t"
      if [ -n "$KEEP" ]; then
        diff <("$OBJDUMP" -d -r -z -m mips:5900 "$out/$t.orig.o" | tail -n +4) \
             <("$OBJDUMP" -d -r -z -m mips:5900 "$out/$t.nat.o" | tail -n +4) > "$out/$t.diff"
      fi
    fi
  done
done
echo "$same/$total identical"
