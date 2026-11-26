#pragma enable_vu0_registers on
#pragma optimization_level 0
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
Q gq1, gq2;
void f1() { Q a = gq1; gq1 = gq2; gq2 = a; }
Q f3(Q *p) { return *p; }
