typedef int qw __attribute__((mode(TI)));
void f1(qw *out, qw *a, qw *b) {
    qw x = *a, y = *b, r = 0;
    asm("vadd.xyzw %0, %1, %2" : "=j"(r) : "j"(x), "j"(y));
    *out = r;
}
qw f2(qw *a) {
    qw x = *a;
    asm("vmul.xyzw %0, %0, %0" : "+j"(x));
    return x;
}
