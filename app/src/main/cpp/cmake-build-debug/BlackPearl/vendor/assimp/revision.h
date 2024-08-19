#ifndef ASSIMP_REVISION_H_INC
#define ASSIMP_REVISION_H_INC

#define GitVersion 0x146dda43
#define GitBranch "andriod_branch"

#define VER_MAJOR 5
#define VER_MINOR 2
#define VER_PATCH 0
#define VER_BUILD 0

#define STR_HELP(x) #x
#define STR(x) STR_HELP(x)

#define VER_FILEVERSION             VER_MAJOR,VER_MINOR,VER_PATCH,VER_BUILD
#if (GitVersion == 0)
#define VER_FILEVERSION_STR         STR(VER_MAJOR) "." STR(VER_MINOR) "." STR(VER_PATCH) "." STR(VER_BUILD)
#else
#define VER_FILEVERSION_STR         STR(VER_MAJOR) "." STR(VER_MINOR) "." STR(VER_PATCH) "." STR(VER_BUILD) " (Commit 146dda43)"
#endif
#define VER_COPYRIGHT_STR           "\xA9 2006-2022"

#ifdef  NDEBUG
#define VER_ORIGINAL_FILENAME_STR   "libassimp.dll"
#else
#define VER_ORIGINAL_FILENAME_STR   "libassimpd.dll"
#endif //  NDEBUG

#endif // ASSIMP_REVISION_H_INC
