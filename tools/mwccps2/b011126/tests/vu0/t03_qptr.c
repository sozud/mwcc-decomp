#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
typedef struct { qw v; } Q;
void f1(Q *dst, Q *src, int n) { Q *p = src; Q *d = dst; while (n--) { *d++ = *p++; } }
void f2(Q *dst, const Q *src) { Q *d = dst; d->v = src->v; d[1] = src[1]; }
qw f3(Q *p) { Q *q = p; return q->v; }
