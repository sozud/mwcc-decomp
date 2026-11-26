#pragma enable_vu0_registers on
typedef int qw __attribute__((mode(TI)));
typedef struct { qw v; } Q;
extern Q get(void);
extern void put(Q);
extern qw geti(void);
extern void puti(qw);
void f1(void) { Q a = get(); put(a); }
void f2(void) { qw a = geti(); puti(a); puti(a); }
void f3(Q *p) { Q a = *p; put(a); *p = get(); }
