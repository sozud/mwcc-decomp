#include "compiler/mwcc_core.h"
#ifndef DRIVER_CLFILES_H
#define DRIVER_CLFILES_H

#include "compiler/common.h"
#include "driver/cmdline.h"
#include "driver/CLDependencies.h"
#include "driver/Memory.h"
#include "driver/oslib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
struct VFile {
    char displayName[32];
    Handle data;
#else
struct VFile {
    char displayName[32];
#if MWCCPSP
    OSHandle data;
#else
    Handle data;
#endif
#endif
    struct VFile* next;
};
struct File {
    struct File* next;
    SInt32 filenum;
    UInt16 segnum;
    OSTime srcmoddate;
    OSTime outmoddate;
    char srcfilename[256];
    char outfilename[256];
    SInt16 outfileowner;
    OSSpec srcfss;
    OSSpec outfss;
    SInt16 writeToDisk;
    SInt16 wroteToDisk;
    SInt16 tempOnDisk;
    struct Plugin* compiler;
    SInt32 dropinflags;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    SInt32 objectflags;
    SInt32 mappingflags;
    SInt16 sourceUsage;
    SInt16 objectUsage;
    Handle textdata;
    Handle objectdata;
    Handle browsedata;
#if MWCC_CORE == MWCC_CORE_B52
    OSSpec tempfss; // b52 (as 3.0.1): File_New allocates 0x8A8, b69's 0x6A4 + an OSSpec
#endif
#else
    SInt32 objectflags;
    SInt32 mappingflags;
    SInt16 sourceUsage;
    SInt16 objectUsage;
#if MWCCPSP
    DriverHandle textdata;
    DriverHandle objectdata;
    DriverHandle browsedata;
#else
    Handle textdata;
    Handle objectdata;
    Handle browsedata;
#endif
#if MWCCPSP
    OSSpec tempfss; // +0x64C: text output the compiler wrote to a temporary file
#endif
#endif
    SInt32 codesize;
    SInt32 udatasize;
    SInt32 idatasize;
    SInt32 compiledlines;
    Boolean recompileDependents;
    Boolean gendebug;
    Boolean hasobjectcode;
    Boolean hasresources;
    Boolean isresourcefile;
    Boolean weakimport;
    Boolean initbefore;
    Boolean mergeintooutput;
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
    // b69's File is 0x6A4 bytes (File_New allocates and clears that much) and
    // every field from deps on is 4 bytes later (Deps_ListDependencies,
    // UCBGetFileInfo, RecordBrowseInfo)
    SInt32 b69_unk;
#else
#if MWCCPSP
    Boolean objectOnDisk; // +0x868: the plugin wrote the object file itself (UCBStoreObjectData)
#endif
#endif
    Deps deps;
    Boolean recordbrowseinfo;
    SInt16 browseFileID;
    char browseoptions[32];
    OSType filetype;
    OSType filecreator;
};
struct Files {
    File* fileList;
    SInt32 fileCount;
};
extern File* File_New(void);
extern Boolean Files_Initialize(Files* files);
extern Boolean Files_Terminate(Files* files);
extern Boolean Files_AddFile(Files* files, File* file);
extern Boolean Files_InsertFile(Files* files, File* file, SInt32 position);
extern File* Files_GetFile(Files* files, SInt32 filenum);
extern File* Files_FindFile(Files* files, OSSpec* spec);
extern int Files_Count(Files* files);
#if MWCC_CORE && MWCC_CORE <= MWCC_CORE_B52
extern Boolean VFiles_Initialize(VFile** list);
extern void VFiles_Terminate(VFile** list);
extern VFile* VFile_New(const char* name, Handle data);
#else
extern Boolean VFiles_Initialize(VFile** list);
extern void VFiles_Terminate(VFile** list);
#if MWCCPSP
extern VFile* VFile_New(const char* name, DriverHandle data);
#else
extern VFile* VFile_New(const char* name, Handle data);
#endif
#endif
extern Boolean VFiles_Add(VFile** list, VFile* entry);
extern VFile* VFiles_Find(VFile* list, const char* name);

#ifdef __cplusplus
}
#endif

#endif
