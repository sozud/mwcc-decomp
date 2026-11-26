#pragma optimization_level 0
#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
void f1(Q *out, Q *a, Q *b) {
    Q x = *a, y = *b, r = *a;
    asm("vadd.xyzw %0, %1, %2" : "=j"(r) : "j"(x), "j"(y));
    *out = r;
}
