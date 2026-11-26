#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
class Q { public: qw v; };
extern Q get();
extern void put(Q);
extern void putp(const Q *);
Q g, h;
Q r1() { return get(); }
Q r2(Q *p) { Q t = *p; return t; }
void c1() { put(get()); }
void c2() { g = get(); h = g; }
void c3(Q *o) { Q t = get(); *o = t; putp(&t); }
Q c4(Q a) { return a; }
void c5(Q *o, int n) { for (int i = 0; i < n; i++) o[i] = get(); }
