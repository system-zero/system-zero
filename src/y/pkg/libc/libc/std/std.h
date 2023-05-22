#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef EOF
#define EOF (-1)
#endif

#ifndef bool
#define bool  _Bool
#define true  1
#define false 0
#endif

#ifndef offsetof
#define offsetof(_type_, _field_) ((size_t) &((_type_ *) 0)->_field_)
#endif

#ifndef container_of
#define container_of(_p_, _c_, _m_) ((_c_ *)((char *) (_p_) - offsetof(_c_, _m_)))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

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

#ifndef MAXSYMLINKS
#define MAXSYMLINKS   20
#endif

#ifndef PATH_SEP
#define PATH_SEP      ':'
#endif

#ifndef DIR_SEP
#define DIR_SEP       '/'
#endif

#ifndef DIR_SEP_STR
#define DIR_SEP_STR   "/"
#endif

#ifndef IS_DIR_SEP
#define IS_DIR_SEP(c_)   (c_ == DIR_SEP)
#endif

#ifndef IS_NOT_DIR_SEP
#define IS_NOT_DIR_SEP(c_) (0 == IS_DIR_SEP (c_))
#endif

#ifndef IS_DIR_ABS
#define IS_DIR_ABS(d_)   IS_DIR_SEP (*d_)
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
