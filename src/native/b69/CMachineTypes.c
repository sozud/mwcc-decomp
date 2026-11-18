// Source-only/native data: the basic type objects. In the hybrid they are
// b69's own data (0x536098..0x53612F: GC 2.0's table with stcustomtype114 at
// 16 bytes and no stvec2x32float); GC's PowerPC CMachine.c, which defines
// them in mweppc, is excluded here. The AltiVec vector types are kept only so
// the GC front end links (b69 never reaches them).
#include "compiler/common.h"
#include "compiler/objects.h"
#include "compiler/types.h"
#include "compiler/CMachine.h"
TypeIntegral stbool = {TYPEINT, 1, IT_BOOL};
TypeIntegral stchar = {TYPEINT, 1, IT_CHAR};
TypeIntegral stsignedchar = {TYPEINT, 1, IT_SCHAR};
TypeIntegral stunsignedchar = {TYPEINT, 1, IT_UCHAR};
TypeIntegral stwchar = {TYPEINT, 2, IT_WCHAR_T};
TypeIntegral stsignedshort = {TYPEINT, 2, IT_SHORT};
TypeIntegral stunsignedshort = {TYPEINT, 2, IT_USHORT};
TypeIntegral stsignedint = {TYPEINT, 4, IT_INT};
TypeIntegral stunsignedint = {TYPEINT, 4, IT_UINT};
TypeIntegral stsignedlong = {TYPEINT, 4, IT_LONG};
TypeIntegral stunsignedlong = {TYPEINT, 4, IT_ULONG};
TypeIntegral stsignedlonglong = {TYPEINT, 8, IT_LONGLONG};
TypeIntegral stunsignedlonglong = {TYPEINT, 8, IT_ULONGLONG};
TypeIntegral stfloat = {TYPEFLOAT, 4, IT_FLOAT};
TypeIntegral stshortdouble = {TYPEFLOAT, 8, IT_SHORTDOUBLE};
TypeIntegral stdouble = {TYPEFLOAT, 8, IT_DOUBLE};
TypeIntegral stlongdouble = {TYPEFLOAT, 8, IT_LONGDOUBLE};
TypeIntegral stcustomtype113 = {TYPEINT, 8, IT_17};
TypeIntegral stcustomtype114 = {TYPEINT, 16, IT_18};
TypeIntegral stvec2x32float = {TYPEFLOAT, 8, IT_VEC2X32FLOAT};

static StructMember stVUC_unsignedchar15 = {NULL, (Type*)&stunsignedchar, NULL, 15, 0};
static StructMember stVUC_unsignedchar14 = {
    &stVUC_unsignedchar15, (Type*)&stunsignedchar, NULL, 14, 0
};
static StructMember stVUC_unsignedchar13 = {
    &stVUC_unsignedchar14, (Type*)&stunsignedchar, NULL, 13, 0
};
static StructMember stVUC_unsignedchar12 = {
    &stVUC_unsignedchar13, (Type*)&stunsignedchar, NULL, 12, 0
};
static StructMember stVUC_unsignedchar11 = {
    &stVUC_unsignedchar12, (Type*)&stunsignedchar, NULL, 11, 0
};
static StructMember stVUC_unsignedchar10 = {
    &stVUC_unsignedchar11, (Type*)&stunsignedchar, NULL, 10, 0
};
static StructMember stVUC_unsignedchar9 = {
    &stVUC_unsignedchar10, (Type*)&stunsignedchar, NULL, 9, 0
};
static StructMember stVUC_unsignedchar8 = {
    &stVUC_unsignedchar9, (Type*)&stunsignedchar, NULL, 8, 0
};
static StructMember stVUC_unsignedchar7 = {
    &stVUC_unsignedchar8, (Type*)&stunsignedchar, NULL, 7, 0
};
static StructMember stVUC_unsignedchar6 = {
    &stVUC_unsignedchar7, (Type*)&stunsignedchar, NULL, 6, 0
};
static StructMember stVUC_unsignedchar5 = {
    &stVUC_unsignedchar6, (Type*)&stunsignedchar, NULL, 5, 0
};
static StructMember stVUC_unsignedchar4 = {
    &stVUC_unsignedchar5, (Type*)&stunsignedchar, NULL, 4, 0
};
static StructMember stVUC_unsignedchar3 = {
    &stVUC_unsignedchar4, (Type*)&stunsignedchar, NULL, 3, 0
};
static StructMember stVUC_unsignedchar2 = {
    &stVUC_unsignedchar3, (Type*)&stunsignedchar, NULL, 2, 0
};
static StructMember stVUC_unsignedchar1 = {
    &stVUC_unsignedchar2, (Type*)&stunsignedchar, NULL, 1, 0
};
static StructMember stVUC_unsignedchar0 = {
    &stVUC_unsignedchar1, (Type*)&stunsignedchar, NULL, 0, 0
};

static StructMember stVSC_signedchar15 = {NULL, (Type*)&stsignedchar, NULL, 15, 0};
static StructMember stVSC_signedchar14 = {&stVSC_signedchar15, (Type*)&stsignedchar, NULL, 14, 0};
static StructMember stVSC_signedchar13 = {&stVSC_signedchar14, (Type*)&stsignedchar, NULL, 13, 0};
static StructMember stVSC_signedchar12 = {&stVSC_signedchar13, (Type*)&stsignedchar, NULL, 12, 0};
static StructMember stVSC_signedchar11 = {&stVSC_signedchar12, (Type*)&stsignedchar, NULL, 11, 0};
static StructMember stVSC_signedchar10 = {&stVSC_signedchar11, (Type*)&stsignedchar, NULL, 10, 0};
static StructMember stVSC_signedchar9 = {&stVSC_signedchar10, (Type*)&stsignedchar, NULL, 9, 0};
static StructMember stVSC_signedchar8 = {&stVSC_signedchar9, (Type*)&stsignedchar, NULL, 8, 0};
static StructMember stVSC_signedchar7 = {&stVSC_signedchar8, (Type*)&stsignedchar, NULL, 7, 0};
static StructMember stVSC_signedchar6 = {&stVSC_signedchar7, (Type*)&stsignedchar, NULL, 6, 0};
static StructMember stVSC_signedchar5 = {&stVSC_signedchar6, (Type*)&stsignedchar, NULL, 5, 0};
static StructMember stVSC_signedchar4 = {&stVSC_signedchar5, (Type*)&stsignedchar, NULL, 4, 0};
static StructMember stVSC_signedchar3 = {&stVSC_signedchar4, (Type*)&stsignedchar, NULL, 3, 0};
static StructMember stVSC_signedchar2 = {&stVSC_signedchar3, (Type*)&stsignedchar, NULL, 2, 0};
static StructMember stVSC_signedchar1 = {&stVSC_signedchar2, (Type*)&stsignedchar, NULL, 1, 0};
static StructMember stVSC_signedchar0 = {&stVSC_signedchar1, (Type*)&stsignedchar, NULL, 0, 0};

static StructMember stVUS_unsignedshort7 = {NULL, (Type*)&stunsignedshort, NULL, 14, 0};
static StructMember stVUS_unsignedshort6 = {
    &stVUS_unsignedshort7, (Type*)&stunsignedshort, NULL, 12, 0
};
static StructMember stVUS_unsignedshort5 = {
    &stVUS_unsignedshort6, (Type*)&stunsignedshort, NULL, 10, 0
};
static StructMember stVUS_unsignedshort4 = {
    &stVUS_unsignedshort5, (Type*)&stunsignedshort, NULL, 8, 0
};
static StructMember stVUS_unsignedshort3 = {
    &stVUS_unsignedshort4, (Type*)&stunsignedshort, NULL, 6, 0
};
static StructMember stVUS_unsignedshort2 = {
    &stVUS_unsignedshort3, (Type*)&stunsignedshort, NULL, 4, 0
};
static StructMember stVUS_unsignedshort1 = {
    &stVUS_unsignedshort2, (Type*)&stunsignedshort, NULL, 2, 0
};
static StructMember stVUS_unsignedshort0 = {
    &stVUS_unsignedshort1, (Type*)&stunsignedshort, NULL, 0, 0
};

static StructMember stVSS_signedshort7 = {NULL, (Type*)&stsignedshort, NULL, 14, 0};
static StructMember stVSS_signedshort6 = {&stVSS_signedshort7, (Type*)&stsignedshort, NULL, 12, 0};
static StructMember stVSS_signedshort5 = {&stVSS_signedshort6, (Type*)&stsignedshort, NULL, 10, 0};
static StructMember stVSS_signedshort4 = {&stVSS_signedshort5, (Type*)&stsignedshort, NULL, 8, 0};
static StructMember stVSS_signedshort3 = {&stVSS_signedshort4, (Type*)&stsignedshort, NULL, 6, 0};
static StructMember stVSS_signedshort2 = {&stVSS_signedshort3, (Type*)&stsignedshort, NULL, 4, 0};
static StructMember stVSS_signedshort1 = {&stVSS_signedshort2, (Type*)&stsignedshort, NULL, 2, 0};
static StructMember stVSS_signedshort0 = {&stVSS_signedshort1, (Type*)&stsignedshort, NULL, 0, 0};

static StructMember stVUL_unsignedlong3 = {NULL, (Type*)&stunsignedlong, NULL, 12, 0};
static StructMember stVUL_unsignedlong2 = {
    &stVUL_unsignedlong3, (Type*)&stunsignedlong, NULL, 8, 0
};
static StructMember stVUL_unsignedlong1 = {
    &stVUL_unsignedlong2, (Type*)&stunsignedlong, NULL, 4, 0
};
static StructMember stVUL_unsignedlong0 = {
    &stVUL_unsignedlong1, (Type*)&stunsignedlong, NULL, 0, 0
};

static StructMember stVSL_signedlong3 = {NULL, (Type*)&stsignedlong, NULL, 12, 0};
static StructMember stVSL_signedlong2 = {&stVSL_signedlong3, (Type*)&stsignedlong, NULL, 8, 0};
static StructMember stVSL_signedlong1 = {&stVSL_signedlong2, (Type*)&stsignedlong, NULL, 4, 0};
static StructMember stVSL_signedlong0 = {&stVSL_signedlong1, (Type*)&stsignedlong, NULL, 0, 0};

static StructMember stVF_float3 = {NULL, (Type*)&stfloat, NULL, 12, 0};
static StructMember stVF_float2 = {&stVF_float3, (Type*)&stfloat, NULL, 8, 0};
static StructMember stVF_float1 = {&stVF_float2, (Type*)&stfloat, NULL, 4, 0};
static StructMember stVF_float0 = {&stVF_float1, (Type*)&stfloat, NULL, 0, 0};

TypeStruct stvectorunsignedchar = {
    TYPESTRUCT, 16, NULL, &stVUC_unsignedchar0, STRUCT_VECTOR_UCHAR, 16
};
TypeStruct stvectorsignedchar = {TYPESTRUCT, 16, NULL, &stVSC_signedchar0, STRUCT_VECTOR_SCHAR, 16};
TypeStruct stvectorboolchar = {TYPESTRUCT, 16, NULL, &stVSC_signedchar0, STRUCT_VECTOR_BCHAR, 16};

TypeStruct stvectorunsignedshort = {
    TYPESTRUCT, 16, NULL, &stVUS_unsignedshort0, STRUCT_VECTOR_USHORT, 16
};
TypeStruct stvectorsignedshort = {
    TYPESTRUCT, 16, NULL, &stVSS_signedshort0, STRUCT_VECTOR_SSHORT, 16
};
TypeStruct stvectorboolshort = {
    TYPESTRUCT, 16, NULL, &stVSS_signedshort0, STRUCT_VECTOR_BSHORT, 16
};

TypeStruct stvectorunsignedlong = {
    TYPESTRUCT, 16, NULL, &stVUL_unsignedlong0, STRUCT_VECTOR_UINT, 16
};
TypeStruct stvectorsignedlong = {TYPESTRUCT, 16, NULL, &stVSL_signedlong0, STRUCT_VECTOR_SINT, 16};
TypeStruct stvectorboollong = {TYPESTRUCT, 16, NULL, &stVSL_signedlong0, STRUCT_VECTOR_BINT, 16};

TypeStruct stvectorfloat = {TYPESTRUCT, 16, NULL, &stVF_float0, STRUCT_VECTOR_FLOAT, 16};

TypeStruct stvectorpixel = {TYPESTRUCT, 16, NULL, &stVUS_unsignedshort0, STRUCT_VECTOR_PIXEL, 16};

TypeStruct stvector = {TYPESTRUCT, 16, NULL, NULL, STRUCT_VECTOR_UINT, 16};
