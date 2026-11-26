#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
typedef struct { qw v; } Q;
void f1(Q *out, Q *a, Q *b) {
    Q x = *a, y = *b, r;
    asm("vadd.xyzw %0, %1, %2" : "=j"(r) : "j"(x), "j"(y));
    *out = r;
}
void f2(qw *out, qw *a) {
    qw x = *a;
    asm("vmul.xyzw %0, %0, %0" : "+j"(x));
    *out = x;
}
