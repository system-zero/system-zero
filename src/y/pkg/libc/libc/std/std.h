#ifndef __WORDSIZE
#if defined(__x86_64__) || defined(__amd64__) || defined(__amd64)
#define __WORDSIZE    64
#elif defined(__i386__) || defined(__i386)
#define __WORDSIZE    32
#else
#error "unimplemented platform"
#endif
#endif

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

#define INTPTR_MIN    INT64_MIN
#define INTPTR_MAX    INT64_MAX
#define UINTPTR_MAX   UINT64_MAX
#define PTRDIFF_MIN   INT64_MIN
#define PTRDIFF_MAX   INT64_MAX

#else

#define INTPTR_MIN    INT32_MIN
#define INTPTR_MAX    INT32_MAX
#define UINTPTR_MAX   UINT32_MAX
#define PTRDIFF_MIN   INT32_MIN
#define PTRDIFF_MAX   INT32_MAX

#endif

#define SSIZE_MAX     LONG_MAX

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__    long unsigned int
#endif

#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ long int
#endif

typedef __SIZE_TYPE__    size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__    uintptr_t;

typedef signed char            int8_t;
typedef short int              int16_t;
typedef int                    int32_t;
typedef unsigned char          uint8_t;
typedef unsigned short int     uint16_t;
typedef unsigned int           uint32_t;

#if __WORDSIZE == 64

typedef long int               int64_t;
typedef unsigned long int      uint64_t;

#else

typedef long long int          int64_t;
typedef unsigned long long int uint64_t;

#endif

typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

#ifdef NULL
#undef NULL
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifdef bool
#undef bool
#endif

#ifdef true
#undef true
#endif

#ifdef false
#undef false
#endif

#define bool  _Bool
#define true  1
#define false 0

#ifdef offsetof
#undef offsetof
#endif

#define offsetof(_type_, _field_) ((size_t) &((_type_ *) 0)->_field_)

#ifdef container_of
#undef container_of
#endif

#define container_of(_p_, _c_, _m_) ((_c_ *)((char *) (_p_) - offsetof(_c_, _m_)))

typedef signed int utf8;

#ifndef IS_UTF8
#define IS_UTF8(__c__) (((__c__) & 0xC0) == 0x80)
#endif

#ifndef ISNOT_UTF8
#define ISNOT_UTF8(__c__) (0 == IS_UTF8(__c__))
#endif

#ifndef NAME_MAX
#define NAME_MAX      255
#endif

#ifndef PATH_MAX
#define PATH_MAX      4096
#endif

#ifndef PIPE_BUF
#define PIPE_BUF      4096
#endif

#ifndef MAXLEN_PATH
#define MAXLEN_PATH   PATH_MAX
#endif

#ifndef MAXLEN_BUF
#define MAXLEN_BUF    4096
#endif

#ifndef MAXLEN_LINE
#define MAXLEN_LINE   MAXLEN_BUF
#endif

#ifndef MAXLEN_WORD
#define MAXLEN_WORD   256
#endif

#ifndef MAXLEN_NAME
#define MAXLEN_NAME   16
#endif

#ifndef PATH_SEP
#define PATH_SEP        ':'
#endif

#ifndef DIR_SEP
#define DIR_SEP         '/'
#endif

#ifndef DIR_SEP_STR
#define DIR_SEP_STR     "/"
#endif

#ifndef IS_DIR_SEP
#define IS_DIR_SEP(c_)   (c_ == DIR_SEP)
#endif

#ifndef IS_DIGIT
#define IS_DIGIT(c_)    ('0' <= (c_) && (c_) <= '9')
#endif

#ifndef IS_CNTRL
#define IS_CNTRL(c_)    ((c_ < 0x20 && c_ >= 0) || c_ == 0x7f)
#endif

#ifndef IS_SPACE
#define IS_SPACE(c_)    ((c_) == ' ' || (c_) == '\t' || (c_) == '\r' || (c_) == '\n')
#endif

#ifndef IS_ALPHA
#define IS_ALPHA(c_)    (((c_) >= 'a' && (c_) <= 'z') || ((c_) >= 'A' && (c_) <= 'Z'))
#endif

#ifndef IS_ALNUM
#define IS_ALNUM(c_)    (IS_ALPHA(c_) || IS_DIGIT(c_))
#endif

#ifndef IS_HEX
#define IS_HEX(c_)      ((IS_DIGIT(c_) || (c_ >= 'a' && c_ <= 'f') || (c_ >= 'A' && c_ <= 'F')))
#endif
