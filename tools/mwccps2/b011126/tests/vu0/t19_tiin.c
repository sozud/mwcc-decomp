typedef int qw __attribute__((mode(TI)));
qw g;
int f1(qw *a) {
    qw x = *a; int d;
    asm("qmfc2 %0, %1" : "=r"(d) : "j"(x));
    return d;
}
int f2(void) {
    int d;
    asm("qmfc2 %0, %1" : "=r"(d) : "j"(g));
    return d;
}
