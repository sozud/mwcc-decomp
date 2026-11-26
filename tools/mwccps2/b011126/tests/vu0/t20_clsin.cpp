typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
Q g;
extern Q get();
int f1(Q *a) {
    Q x = *a; int d;
    asm("qmfc2 %0, %1" : "=r"(d) : "j"(x));
    return d;
}
int f2() {
    int d;
    asm("qmfc2 %0, %1" : "=r"(d) : "j"(g));
    return d;
}
int f3() {
    Q x = get(); int d;
    asm("qmfc2 %0, %1" : "=r"(d) : "j"(x));
    return d;
}
void f4(Q *o, Q *a) {
    Q x = *a, r;
    asm("vmove.xyzw %0, %1" : "=j"(r) : "j"(x));
    *o = r;
    g = r;
}
void f5(Q *o, Q *a, int n) {
    Q *p = a + n;
    Q r;
    asm("vmove.xyzw %0, %1" : "=j"(r) : "j"(*p));
    *o = r;
}
