#ifndef FILETYPE_HDR
#define FILETYPE_HDR

#define MAXLEN_FILETYPE 256

#define FILETYPE_ERROR            -1
#define FILETYPE_NON_EXISTANT      0
#define FILETYPE_EMPTY_FILE        1
#define FILETYPE_TXT               2
#define FILETYPE_DIRECTORY         3
#define FILETYPE_CHAR_DEVICE       4
#define FILETYPE_BLOCK_DEVICE      5
#define FILETYPE_NAMED_PIPE        6
#define FILETYPE_SYMLINK           7
#define FILETYPE_SOCKET            8

#define FILETYPE_ELF_OBJECT       20
#define FILETYPE_STATIC_OBJECT    21
#define FILETYPE_WEB_ASSEMBLY_BIN 22
#define FILETYPE_LUA_BYTECODE     23
#define FILETYPE_CHROME_EXTENSION 24
#define FILETYPE_POSIX_SH         25
#define FILETYPE_ZSH              26
#define FILETYPE_BASH             27
#define FILETYPE_SCRIPT           28

#define FILETYPE_BZIP             50
#define FILETYPE_GZIP             51
#define FILETYPE_COMPRESSED       52
#define FILETYPE_ZIP              53
#define FILETYPE_XZ               54
#define FILETYPE_TAR_POSIX        55
#define FILETYPE_TAR_POSIX_GNU    56
#define FILETYPE_TAR              57
#define FILETYPE_IS_TAR(_v_) (FILETYPE_TAR_POSIX <= _v_ && _v_ <= FILETYPE_TAR)

#define FILETYPE_PS               70
#define FILETYPE_PDF              71

#define FILETYPE_GIF              90
#define FILETYPE_PNG              91
#define FILETYPE_JPEG             92
#define FILETYPE_TTF              93

#define FILETYPE_FLAC             110
#define FILETYPE_OGG              111
#define FILETYPE_MATROSKA_WEBM    112
#define FILETYPE_MP3_WITH_ID3V2   113
#define FILETYPE_MP4              114

int filetype (const char *, char[MAXLEN_FILETYPE]);
int filetype_from_string (const char *, size_t, char[MAXLEN_FILETYPE]);

#endif /* FILETYPE_HDR */
