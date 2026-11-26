typedef int qw __attribute__((mode(TI)));
void f2(qw *out, qw *a) {
    qw x = *a;
    asm("vmul.xyzw %0, %0, %0" : "+j"(x));
    *out = x;
}
void f3(qw *out, qw *a, qw *b) {
    qw x = *a, y = *b, r;
    asm("vadd.xyzw %0, %1, %2" : "=j"(r) : "j"(x), "j"(y));
    *out = r;
}
