# wp.py STOPADDR N WATCH -- CMD... : hardware write watchpoint on byte WATCH; int3 at STOPADDR,
# stop at its Nth hit; prints the last 6 writes (eip after the write, byte value, 12 stack words)
import collections
import ctypes
import os
import signal
import sys

libc = ctypes.CDLL(None, use_errno=True)
libc.ptrace.restype = ctypes.c_long
libc.ptrace.argtypes = [ctypes.c_long, ctypes.c_long, ctypes.c_void_p, ctypes.c_void_p]
TRACEME, PEEK, PEEKUSER, POKE, POKEUSER, CONT, KILL, STEP, GETREGS, SETREGS, SYSCALL = (
    0,
    2,
    3,
    5,
    6,
    7,
    8,
    9,
    12,
    13,
    24,
)
DR = 848
stop, nstop, watch = int(sys.argv[1], 16), int(sys.argv[2]), int(sys.argv[3], 16)
cmd = sys.argv[sys.argv.index("--") + 1 :]
pid = os.fork()
if pid == 0:
    libc.ptrace(TRACEME, 0, None, None)
    devnull = os.open("/dev/null", os.O_WRONLY)
    os.dup2(devnull, 2)
    os.dup2(devnull, 1)
    os.execvp(cmd[0], cmd)
os.waitpid(pid, 0)


def peek(a):
    return libc.ptrace(PEEK, pid, ctypes.c_void_p(a), None)


def poke(a, v):
    libc.ptrace(POKE, pid, ctypes.c_void_p(a), ctypes.c_void_p(v & 0xFFFFFFFFFFFFFFFF))


while True:
    libc.ptrace(SYSCALL, pid, None, None)
    p, st = os.waitpid(pid, 0)
    if os.WIFEXITED(st):
        sys.exit("exited before mapping")
    ctypes.set_errno(0)
    v = peek(stop)
    if ctypes.get_errno() == 0 and v != 0:
        break
orig = peek(stop)
poke(stop, (orig & ~0xFF) | 0xCC)
libc.ptrace(POKEUSER, pid, ctypes.c_void_p(DR), ctypes.c_void_p(watch))
libc.ptrace(POKEUSER, pid, ctypes.c_void_p(DR + 7 * 8), ctypes.c_void_p(0x10001))
last = collections.deque(maxlen=6)
hits = 0
nw = 0
libc.ptrace(CONT, pid, None, None)
while True:
    p, st = os.waitpid(pid, 0)
    if os.WIFEXITED(st) or os.WIFSIGNALED(st):
        print("exited")
        break
    sig = os.WSTOPSIG(st)
    if sig != signal.SIGTRAP:
        libc.ptrace(CONT, pid, None, ctypes.c_void_p(sig))
        continue
    regs = (ctypes.c_ulonglong * 27)()
    libc.ptrace(GETREGS, pid, None, ctypes.byref(regs))
    dr6 = libc.ptrace(PEEKUSER, pid, ctypes.c_void_p(DR + 6 * 8), None)
    if dr6 & 1:
        libc.ptrace(POKEUSER, pid, ctypes.c_void_p(DR + 6 * 8), ctypes.c_void_p(0))
        esp = regs[19] & 0xFFFFFFFF
        nw += 1
        last.append(
            (
                nw,
                regs[16] & 0xFFFFFFFF,
                peek(watch & ~3) >> (8 * (watch & 3)) & 0xFF,
                [peek(esp + 4 * k) & 0xFFFFFFFF for k in range(16)],
            )
        )
        libc.ptrace(CONT, pid, None, None)
        continue
    ip = regs[16] - 1
    if ip == stop:
        hits += 1
        if hits == nstop:
            for w in last:
                print("W#%d eip=%08x val=%d stack=%s" % (w[0], w[1], w[2], " ".join("%08x" % s for s in w[3])))
            libc.ptrace(KILL, pid, None, None)
            break
        regs[16] = ip
        libc.ptrace(SETREGS, pid, None, ctypes.byref(regs))
        poke(ip, orig)
        libc.ptrace(STEP, pid, None, None)
        os.waitpid(pid, 0)
        poke(ip, (orig & ~0xFF) | 0xCC)
    libc.ptrace(CONT, pid, None, None)
