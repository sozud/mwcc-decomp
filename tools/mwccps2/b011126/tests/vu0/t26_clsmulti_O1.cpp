#pragma optimization_level 1
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
void m1(Q *o, const Q *a, const Q *b, const Q *c) {
    Q x = *a, y = *b, z = *c, r, s;
    asm("vadd.xyzw %0, %1, %2" : "=j"(r) : "j"(x), "j"(y));
    asm("vmul.xyzw %0, %1, %2" : "=j"(s) : "j"(r), "j"(z));
    o[0] = r; o[1] = s;
}
void m2(Q *o, const Q *a, int n) {
    Q acc = *a;
    for (int i = 1; i < n; i++) {
        Q t = a[i];
        asm("vadd.xyzw %0, %0, %1" : "+j"(acc) : "j"(t));
    }
    *o = acc;
}
Q m3(const Q *a) {
    Q x = *a, r;
    asm("vsub.xyzw %0, %1, %1" : "=j"(r) : "j"(x));
    return r;
}
void m4(Q *o, Q *a) {
    Q x = *a;
    asm("vmul.xyzw %0, %0, %0" : "+j"(x));
    asm("vmul.xyzw %0, %0, %0" : "+j"(x));
    *o = x;
    *a = x;
}
