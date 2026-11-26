#pragma enable_vu0_registers on
#pragma peephole on
typedef int qw __attribute__((mode(TI)));
qw g;
int f1(qw *a) { int d; asm("qmfc2 %0, %1" : "=r"(d) : "j"(g)); return d; }
int f3(qw *a) { asm("vnop" : "+j"(g)); return 0; }
int f4(qw *a) { qw x = *a; asm("vnop" : "+j"(x)); *a = x; return 0; }
int f5(qw *a) { int d; asm("qmfc2 %0, %1" : "=r"(d) : "j"(a[1])); return d; }
