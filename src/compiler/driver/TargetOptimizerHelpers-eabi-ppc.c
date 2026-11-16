#include "compiler/mwcc_core.h"
#include "driver/TargetOptimizerHelpers-eabi-ppc.h"
#include "compiler/CParser.h"
#include "driver/pref_structs.h"
#include "driver/StringUtils.h"

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
// b69 0x421920/0x421950: 3.0.1's MIPS version (tools/mwccpsp's MWCCPSP
// branch, the PS2 display): the only target flag is -opt [no]intrinsics, in
// the "PS2 MIPS CodeGen" panel (0x510B94, 0x14 bytes) at +0x0D
extern struct {
    UInt8 unk00[0xD];
    UInt8 inline_intrinsics;
} pMIPSCodeGen;

#endif
#if MWCCPSP
// 3.0.1 PSP: the only target optimization flag is -opt [no]intrinsics,
// kept in the MIPS code generator prefs (CodeGen.c: data_005B0A78, +0x0D)
extern struct {
    UInt8 unk00[0xD];
    UInt8 inline_intrinsics;
} data_005B0A78;

int TargetSetOptFlags(short val, Boolean set) {
    switch (val) {
        case 'In':
            data_005B0A78.inline_intrinsics = set;
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

void TargetDisplayOptimizationOptions(Handle txt) {
#if MWCCPS2
    if (data_005B0A78.inline_intrinsics) {
        HPrintF(
            txt, "\t- inline intrinsics %s\n", (data_005B0A78.inline_intrinsics == 1) ? "on" : "off"
        );
    }
#else
    UInt8 intrinsics;

    if ((intrinsics = data_005B0A78.inline_intrinsics)) {
        HPrintF(txt, "\t- inline intrinsics %s\n", (intrinsics == 1) ? "on" : "off");
    }
#endif
}
#else
int TargetSetOptFlags(short val, Boolean set) {
    switch (val) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
        case 'In':
            pMIPSCodeGen.inline_intrinsics = set;
#else
        case 'Pe':
            pBackEnd.peephole = set;
#endif
            break;
#if !MWCC_CORE || MWCC_CORE > MWCC_CORE_B52
        case 'Sn':
            pBackEnd.schedule = FALSE;
            break;
        case 'Sh':
            pBackEnd.schedule = set;
            break;
#endif
        default:
            return FALSE;
    }

    return TRUE;
}

void TargetDisplayOptimizationOptions(Handle txt) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    if (pMIPSCodeGen.inline_intrinsics) {
        HPrintF(
            txt, "\t- inline intrinsics %s\n", (pMIPSCodeGen.inline_intrinsics == 1) ? "on" : "off"
        );
#else
    int p;

    HPrintF(txt, "\t- peephole optimizations %s\n", pBackEnd.peephole ? "on" : "off");
    if (!pBackEnd.schedule) {
        HPrintF(txt, "\t- no instruction scheduling\n");
    } else {
        HPrintF(
            txt,
            "\t- schedule for %s\n",
            ((p = pBackEnd.processor) == CPU_Generic) ? "generic PPC"
                : (p == CPU_PPC401)                   ? "401"
                : (p == CPU_PPC403)                   ? "403"
                : (p == CPU_PPC505)                   ? "505"
                : (p == CPU_PPC509)                   ? "509"
                : (p == CPU_PPC555)                   ? "555"
                : (p == CPU_PPC601)                   ? "601"
                : (p == CPU_PPC602)                   ? "602"
                : (p == CPU_PPC603)                   ? "603"
                : (p == CPU_PPC603e)                  ? "603e"
                : (p == CPU_PPC604)                   ? "604"
                : (p == CPU_PPC604e)                  ? "604e"
                : (p == CPU_PPC740)                   ? "740"
                : (p == CPU_PPC750)                   ? "750"
                : (p == CPU_PPC801)                   ? "801"
                : (p == CPU_PPC821)                   ? "821"
                : (p == CPU_PPC823)                   ? "823"
                : (p == CPU_PPC850)                   ? "850"
                : (p == CPU_PPC860)                   ? "860"
                : (p == CPU_PPC8240)                  ? "8240"
                : (p == CPU_PPC8260)                  ? "8260"
                : (p == CPU_PPC7400)                  ? "7400"
                : (p == CPU_PPCGEKKO)                 ? "GEKKO"
                : (p == CPU_PPCELF)                   ? "e500"
                : (p == CPU_PPC7450)                  ? "7450"
                                                      : "???"
        );
#endif
    }
}

#endif
#if MWCCPSP

#endif
void TargetSetPragmaOptimizationsToUnspecified(void) {}
