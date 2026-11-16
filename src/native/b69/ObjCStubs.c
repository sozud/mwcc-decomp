// Source-only/native stubs: b69 has no Objective-C (its TypeClass has no
// objcinfo), but the GC 2.4.7 front end still names CObjC.c's hooks.
// Nothing reaches them; each stops the compiler if one ever runs.
#include <stdlib.h>

void* cobjc_classdefs;
void* cobjc_classrefcount;
void* cobjc_currentclass;
void* cobjc_protocols;
void* cobjc_selhashtable;
void* cobjc_selrefcount;
void* cobjc_stringcount;
void* cobjc_type_class;
void* cobjc_type_id;
void* cobjc_type_sel;

int CObjC_CheckModernSendMessage(void) {
    abort();
    return 0;
}
int CObjC_Cleanup(void) {
    abort();
    return 0;
}
int CObjC_Delete(void) {
    abort();
    return 0;
}
int CObjC_GenerateModule(void) {
    abort();
    return 0;
}
int CObjC_GetObjCType_id(void) {
    abort();
    return 0;
}
int CObjC_IsCompatibleType(void) {
    abort();
    return 0;
}
int CObjC_IsType_id(void) {
    abort();
    return 0;
}
int CObjC_ParseAtExpression(void) {
    abort();
    return 0;
}
int CObjC_ParseClassDeclaration(void) {
    abort();
    return 0;
}
int CObjC_ParseDefs(void) {
    abort();
    return 0;
}
int CObjC_ParseEncodeExpression(void) {
    abort();
    return 0;
}
int CObjC_ParseID(void) {
    abort();
    return 0;
}
int CObjC_ParseImplementation(void) {
    abort();
    return 0;
}
int CObjC_ParseInterface(void) {
    abort();
    return 0;
}
int CObjC_ParseMessageExpression(void) {
    abort();
    return 0;
}
int CObjC_ParseProtocol(void) {
    abort();
    return 0;
}
int CObjC_ParseProtocolExpression(void) {
    abort();
    return 0;
}
int CObjC_ParseSelectorExpression(void) {
    abort();
    return 0;
}
int CObjC_Setup(void) {
    abort();
    return 0;
}
