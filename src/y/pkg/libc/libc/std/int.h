#define MB_LEN_MAX    16
#define OPEN_MAX      1024
#define CHAR_BIT      8
#define SHRT_BIT      16
#define INT_BIT       32
#define LONGLONG_BIT  64

#define SCHAR_MIN     (-128)
#define SCHAR_MAX     127
#define UCHAR_MAX     255

#ifdef __CHAR_UNSIGNED__
#define CHAR_MIN      0
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#endif

#if __WORDSIZE == 64

#define _BITSIZE      64
#define LONG_BIT      64

#define LONG_MAX      9223372036854775807L
#define ULONG_MAX     18446744073709551615UL

#define __INT64_C(c)  c ## L
#define __UINT64_C(c) c ## UL

#else

#define _BITSIZE      32
#define LONG_BIT      32
#define LONG_MAX      2147483647L
#define ULONG_MAX     4294967295UL

#define __INT64_C(c)  c ## LL
#define __UINT64_C(c) c ## ULL

#endif

#define LONG_MIN      (-LONG_MAX - 1L)
#define ULLONG_MAX    18446744073709551615ULL
#define LLONG_MAX     9223372036854775807LL
#define LLONG_MIN     (-LLONG_MAX - 1LL)

#define SHRT_MIN      (-32768)
#define SHRT_MAX      32767
#define USHRT_MAX     65535

#define INT_MIN       (-2147483647-1)
#define INT_MAX       2147483647
#define UINT_MAX      4294967295U

#define INT8_MIN      (-128)
#define INT16_MIN     (-32768)
#define INT32_MIN     (-2147483647-1)
#define INT64_MIN     (__INT64_C(-9223372036854775807)-1)

#define INT8_MAX      (127)
#define INT16_MAX     (32767)
#define INT32_MAX     (2147483647)
#define INT64_MAX     (__INT64_C(9223372036854775807))

#define UINT8_MAX     (255U)
#define UINT16_MAX    (65535U)
#define UINT32_MAX    (4294967295U)
#define UINT64_MAX    (__UINT64_C(18446744073709551615))

#define INTMAX_MIN    INT64_MIN
#define INTMAX_MAX    INT64_MAX
#define UINTMAX_MAX   UINT64_MAX

#if __WORDSIZE == 64

#define SIZE_MAX      (18446744073709551615UL)
#define INTPTR_MIN    INT64_MIN
#define INTPTR_MAX    INT64_MAX
#define UINTPTR_MAX   UINT64_MAX
#define PTRDIFF_MIN   INT64_MIN
#define PTRDIFF_MAX   INT64_MAX

#else

#define SIZE_MAX      (4294967295U)
#define INTPTR_MIN    INT32_MIN
#define INTPTR_MAX    INT32_MAX
#define UINTPTR_MAX   UINT32_MAX
#define PTRDIFF_MIN   INT32_MIN
#define PTRDIFF_MAX   INT32_MAX

#endif

#define SSIZE_MAX     LONG_MAX

#ifndef __SIZE_TYPE__
  #if __WORDSIZE == 64
  #define __SIZE_TYPE__     unsigned long int
  #else
  #define __SIZE_TYPE__     unsigned long long int
  #endif
#endif

#ifndef __PTRDIFF_TYPE__
  #if __WORDSIZE == 64
  #define __PTRDIFF_TYPE__  signed long int
  #else
  #define __PTRDIFF_TYPE__  signed long long int
  #endif
#endif

typedef __SIZE_TYPE__      size_t;
typedef __SIZE_TYPE__      uintptr_t;
typedef __PTRDIFF_TYPE__   ssize_t;
typedef __PTRDIFF_TYPE__   ptrdiff_t;
typedef __PTRDIFF_TYPE__   intptr_t;

typedef signed char        int8_t;
typedef short int          int16_t;
typedef int                int32_t;
typedef unsigned char      uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int       uint32_t;

typedef unsigned char      uint_fast8_t;

#if __WORDSIZE == 64

typedef long int           int_fast16_t;
typedef long int           int_fast32_t;
typedef long int           int_fast64_t;
typedef unsigned long int  uint_fast16_t;
typedef unsigned long int  uint_fast32_t;
typedef unsigned long int  uint_fast64_t;

#else

typedef int                int_fast16_t;
typedef int                int_fast32_t;
typedef long long int      int_fast64_t;
typedef unsigned int       uint_fast16_t;
typedef unsigned int       uint_fast32_t;
typedef unsigned long long int uint_fast64_t;

#endif

#if __WORDSIZE == 64

typedef signed long int   int64_t;
typedef unsigned long int uint64_t;

#else

typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;

#endif

typedef uint8_t   byte;
typedef uint16_t  word;
typedef uint32_t  dword;
typedef uint64_t  qword;

typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

typedef signed int utf8;

#define PAGESIZE 4096

#if __WORDSIZE == 64

#define PRId64  "ld"
#define PRIu64  "lu"
#define PRIx64  "lx"
#define PRIo64  "lo"
#define PRIdPTR "ld"
#define PRIuPTR "lu"

#else

#define PRId64  "lld"
#define PRIu64  "llu"
#define PRIx64  "llx"
#define PRIo64  "llo"
#define PRIdPTR "d"
#define PRIuPTR "u"

#endif

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
