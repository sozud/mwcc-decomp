#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
typedef struct { qw v; } Q;
Q gq1, gq2;
void f1(void) { Q a = gq1; gq1 = gq2; gq2 = a; }
void f2(Q *p, Q *q) { Q t = *p; *p = *q; *q = t; }
Q f3(Q *p) { return *p; }
void f4(Q x, Q *out) { *out = x; }
Q f5(void) { Q a; a = gq1; return a; }
