#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
qw g1, g2;
void f1(void) { qw a = g1; qw b = g2; g1 = b; g2 = a; }
void f2(qw *p, qw *q) { qw t = *p; *p = *q; *q = t; }
qw f3(qw *p) { return *p; }
void f4(qw x, qw *out) { *out = x; }
