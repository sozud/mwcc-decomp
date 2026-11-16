#include "compiler/mwcc_core.h"
// Source-only/native data: the back end's expression dispatch (data_005D0140,
// 3.0.1 0x5D0140, b145 0x5E88A0), indexed by node type. The copied back end
// is 3.0.1's, whose ENodeType has EMIN/EMAX and ESCOPEBEGIN..ESTMT that GC's
// (b69's) lacks, in another order: this is b145's table reordered by node
// name for b69's enum. The b69-only kinds (EOBJLIST, EVECTOR128CONST) take
// the table's default handler, func_00494940, as b69's own table (0x53C6A0,
// which has the same groups of equal entries) does.
#include "compiler/common.h"
#include "compiler/enode.h"

extern void func_00493B50(void);
extern void func_00493BB0(void);
extern void func_00493D90(void);
extern void func_00493EF0(void);
extern void func_00493F90(void);
extern void func_00494940(void);
extern void func_00494960(void);
extern void func_00494AA0(void);
extern void func_00494AE0(void);
extern void func_004951D0(void);
extern void func_004951F0(void);
extern void func_00495410(void);
extern void func_004954B0(void);
extern void func_00495500(void);
extern void func_00496C70(void);
extern void func_00496CB0(void);
extern void func_00496D50(void);
extern void func_00497360(void);
extern void func_004973C0(void);
extern void func_00497420(void);
extern void func_00497A90(void);
extern void func_00497E50(void);
extern void func_004980C0(void);
extern void func_00498590(void);
extern void func_00499200(void);
extern void func_00499820(void);
extern void func_0049A740(void);
extern void func_0049AEF0(void);
extern void func_0049AF50(void);
extern void func_0049B1D0(void);
extern void func_0049B770(void);
extern void func_0049B870(void);
extern void func_0049B8D0(void);
extern void func_0049C160(void);
extern void func_0049C1E0(void);

void (*data_005D0140[MAXEXPR])(void) = {
    func_0049B8D0, // EPOSTINC (3.0.1 0)
    func_0049B8D0, // EPOSTDEC (3.0.1 1)
    func_0049B870, // EPREINC (3.0.1 2)
    func_0049B870, // EPREDEC (3.0.1 3)
    func_0049B770, // EINDIRECT (3.0.1 4)
    func_0049B1D0, // EMONMIN (3.0.1 5)
    func_0049AF50, // EBINNOT (3.0.1 6)
    func_00493EF0, // ELOGNOT (3.0.1 7)
    func_0049AEF0, // EFORCELOAD (3.0.1 8)
    func_0049A740, // EMUL (3.0.1 9)
    func_00494940, // EMULV (3.0.1 10)
    func_00499820, // EDIV (3.0.1 11)
    func_00499200, // EMODULO (3.0.1 12)
    func_00494940, // EADDV (3.0.1 13)
    func_00494940, // ESUBV (3.0.1 14)
    func_00498590, // EADD (3.0.1 15)
    func_004980C0, // ESUB (3.0.1 16)
    func_00497E50, // ESHL (3.0.1 17)
    func_00497A90, // ESHR (3.0.1 18)
    func_00493F90, // ELESS (3.0.1 19)
    func_00493F90, // EGREATER (3.0.1 20)
    func_00493F90, // ELESSEQU (3.0.1 21)
    func_00493F90, // EGREATEREQU (3.0.1 22)
    func_00493F90, // EEQU (3.0.1 23)
    func_00493F90, // ENOTEQU (3.0.1 24)
    func_00497420, // EAND (3.0.1 25)
    func_004973C0, // EXOR (3.0.1 26)
    func_00497360, // EOR (3.0.1 27)
    func_00493D90, // ELAND (3.0.1 28)
    func_00493D90, // ELOR (3.0.1 29)
    func_00496D50, // EASS (3.0.1 30)
    func_00494940, // EMULASS (3.0.1 31)
    func_00494940, // EDIVASS (3.0.1 32)
    func_00494940, // EMODASS (3.0.1 33)
    func_00494940, // EADDASS (3.0.1 34)
    func_00494940, // ESUBASS (3.0.1 35)
    func_00494940, // ESHLASS (3.0.1 36)
    func_00494940, // ESHRASS (3.0.1 37)
    func_00494940, // EANDASS (3.0.1 38)
    func_00494940, // EXORASS (3.0.1 39)
    func_00494940, // EORASS (3.0.1 40)
    func_00496C70, // ECOMMA (3.0.1 41)
    func_00494940, // EPMODULO (3.0.1 44)
    func_00494940, // EROTL (3.0.1 45)
    func_00494940, // EROTR (3.0.1 46)
    func_00494940, // EBCLR (3.0.1 47)
    func_00494940, // EBTST (3.0.1 48)
    func_00494940, // EBSET (3.0.1 49)
#if MWCC_CORE == MWCC_CORE_B52
    func_00494940, // EBTST (48)
    func_00494940, // EBSET (49)
#endif
    func_00495500, // ETYPCON (3.0.1 50)
    func_004954B0, // EBITFIELD (3.0.1 51)
    func_00495410, // EINTCONST (3.0.1 52)
    func_004951F0, // EFLOATCONST (3.0.1 53)
    func_004951D0, // ESTRINGCONST (3.0.1 54)
    func_00494AE0, // ECOND (3.0.1 55)
    func_00494AA0, // EFUNCCALL (3.0.1 56)
    func_00494940, // EFUNCCALLP (3.0.1 57)
    func_00494960, // EOBJREF (3.0.1 58)
#if MWCC_CORE != MWCC_CORE_B52
    func_00494940, // EMFPOINTER (3.0.1 64)
#endif
    func_00493BB0, // ENULLCHECK (3.0.1 59)
    func_00493B50, // EPRECOMP (3.0.1 60)
#if MWCC_CORE == MWCC_CORE_B52
    func_00494940, // ELABEL (61)
    func_00494940, // ESCOPEBEGIN (62)
    func_00494940, // ESCOPEEND (63)
    func_00494940, // EMFPOINTER (64)
    func_00494940, // ETEMP (65)
    func_00494940, // ELOCOBJ (66)
#endif
    func_00494940, // ETEMP (3.0.1 65)
    func_00494940, // EARGOBJ (3.0.1 67)
    func_00494940, // ELOCOBJ (3.0.1 66)
    func_00494940, // ELABEL (3.0.1 61)
    func_00494940, // ESETCONST (3.0.1 68)
    func_00494940, // ENEWEXCEPTION (3.0.1 69)
    func_00494940, // ENEWEXCEPTIONARRAY (3.0.1 70)
    func_00494940, // EINITTRYCATCH (3.0.1 71)
    func_00494940, // EOBJLIST (not in 3.0.1's table)
    func_00494940, // EMEMBER (3.0.1 72)
    func_00494940, // ETEMPLDEP (3.0.1 73)
    func_00494940, // EINSTRUCTION (3.0.1 78)
    func_0049C1E0, // EDEFINE (3.0.1 79)
    func_0049C160, // EREUSE (3.0.1 80)
    func_00496CB0, // EASSBLK (3.0.1 81)
    func_00494940, // ECONDASS (3.0.1 82)
};
