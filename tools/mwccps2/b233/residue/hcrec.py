#!/usr/bin/env python3
# hcrec.py OUT -- wibo mwccps2.exe ARGS: every hascall test in 2.3.3's
# call_function (0x49E830) as "site value" lines, for MWCCPS2_B233_HASCALL
# (FunctionCalls.c). Sites: 1 0x49E925 [ecx+5], 2 0x49E94F, 3 0x49E97B,
# 4 0x49EFFB, 5 0x49F06C ([eax+5]).
import ctypes
import os
import sys

libc = ctypes.CDLL(None, use_errno=True)
libc.ptrace.restype = ctypes.c_long
libc.ptrace.argtypes = [ctypes.c_long, ctypes.c_long, ctypes.c_void_p, ctypes.c_void_p]
TRACEME, PEEK, POKE, CONT, STEP, GETREGS, SETREGS, SYSCALL = 0, 2, 5, 7, 9, 12, 13, 24
SITES = {
    0x49E925: (1, 11),
    0x49E94F: (2, 10),
    0x49E97B: (3, 10),
    0x49EFFB: (4, 10),
    0x49F06C: (5, 10),
}  # reg index: ecx 11, eax 10
out = open(sys.argv[1], "w")
cmd = sys.argv[sys.argv.index("--") + 1 :]
pid = os.fork()
if pid == 0:
    libc.ptrace(TRACEME, 0, None, None)
    d = os.open("/dev/null", os.O_WRONLY)
    os.dup2(d, 1)
    os.dup2(d, 2)
    os.execvp(cmd[0], cmd)
os.waitpid(pid, 0)
peek = lambda x: libc.ptrace(PEEK, pid, ctypes.c_void_p(x), None) & 0xFFFFFFFFFFFFFFFF
poke = lambda x, v: libc.ptrace(POKE, pid, ctypes.c_void_p(x), ctypes.c_void_p(v))
first = min(SITES)
while True:
    libc.ptrace(SYSCALL, pid, None, None)
    _, st = os.waitpid(pid, 0)
    if os.WIFEXITED(st):
        sys.exit("exited")
    ctypes.set_errno(0)
    v = peek(first)
    if ctypes.get_errno() == 0 and v:
        break


# breakpoints are single bytes: re-read the live word each time
def setb(x, b):
    w = peek(x)
    poke(x, (w & ~0xFF) | b)


orig = {a: peek(a) & 0xFF for a in SITES}
for a in SITES:
    setb(a, 0xCC)
libc.ptrace(CONT, pid, None, None)
n = 0
while True:
    _, st = os.waitpid(pid, 0)
    if os.WIFEXITED(st) or os.WIFSIGNALED(st):
        break
    if os.WSTOPSIG(st) != 5:
        libc.ptrace(CONT, pid, None, ctypes.c_void_p(os.WSTOPSIG(st)))
        continue
    r = (ctypes.c_ulonglong * 27)()
    libc.ptrace(GETREGS, pid, None, ctypes.byref(r))
    at = (r[16] - 1) & 0xFFFFFFFF
    if at not in SITES:
        libc.ptrace(CONT, pid, None, None)
        continue
    site, ri = SITES[at]
    node = r[ri] & 0xFFFFFFFF
    out.write(f"{site} {peek(node + 5) & 0xFF}\n")
    n += 1
    r[16] -= 1
    libc.ptrace(SETREGS, pid, None, ctypes.byref(r))
    setb(at, orig[at])
    libc.ptrace(STEP, pid, None, None)
    os.waitpid(pid, 0)
    setb(at, 0xCC)
    libc.ptrace(CONT, pid, None, None)
print(n, "tests")
