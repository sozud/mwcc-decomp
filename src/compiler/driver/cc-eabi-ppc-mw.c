#include "compiler/mwcc_core.h"
#include "driver/cc-eabi-ppc-mw.h"
#include "driver/cc-eabi-ppc.h"
#include "driver/CWPlugins.h"
#include "driver/libimp-eabi-ppc.h"
#include "driver/oslib.h"

void GetStaticTarget(OSType* cpu, OSType* os) {
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    *cpu = 'mips';
    *os = 'psx2';
#else
    *cpu = 'ePPC';
    *os = 'EABI';
#endif
}

void GetStaticPluginType(OSType* language, OSType* plugintype) {
    *language = Lang_Any;
    *plugintype = CWDROPINCOMPILERTYPE;
}

void GetStaticParserPluginType(OSType* style) {
    *style = 'Seep';
}

int RegisterStaticTargetPlugins(void) {
    return RegisterStaticCompilerPlugin() && RegisterStaticLibImporterPlugin();
}

int RegisterStaticTargetResources(void) {
    return RegisterCompilerResources() && RegisterLibImporterResources();
}
