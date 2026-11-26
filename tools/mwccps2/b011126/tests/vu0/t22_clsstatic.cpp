#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
extern Q get();
static Q sg;
void s1() { static Q s; s = get(); sg = s; }
void s2(Q *o) { static Q s; Q r; asm("vmove.xyzw %0, %1" : "=j"(r) : "j"(s)); *o = r; }
void s3(Q *o) { Q r; asm("vmove.xyzw %0, %1" : "=j"(r) : "j"(sg)); sg = r; *o = sg; }
