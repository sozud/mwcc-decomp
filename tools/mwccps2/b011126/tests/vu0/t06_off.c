typedef int qw __attribute__((mode(TI)));
typedef struct { qw v; } Q;
Q gq1, gq2;
void f1(void) { Q a = gq1; gq1 = gq2; gq2 = a; }
void f2(Q *p, Q *q) { Q t = *p; *p = *q; *q = t; }
