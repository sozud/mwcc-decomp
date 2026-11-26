#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
extern Q get();
extern void put(Q);
void f1() { Q a = get(); put(a); }
void f3(Q *p) { Q a = *p; put(a); *p = get(); }
Q f4(Q a, Q b) { put(b); return a; }
