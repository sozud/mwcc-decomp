#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
void f1(Q *out, Q *a) {
    Q x = *a;
    asm {
        vadd.xyzw x, x, x
    }
    *out = x;
}
