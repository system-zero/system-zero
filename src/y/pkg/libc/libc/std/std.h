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
#define MIN(_a_, _b_) (((_a_) < (_b_)) ? (_a_) : (_b_))
#endif

#ifndef MAX
#define MAX(_a_, _b_) (((_a_) > (_b_)) ? (_a_) : (_b_))
#endif

#ifndef ARRAY_LEN
#define ARRAY_LEN(_a_) (sizeof ((_a__) / sizeof ((_a_)[0]))
#endif

#ifndef IS_UTF8
#define IS_UTF8(_c_) (((_c_) & 0xC0) == 0x80)
#endif

#ifndef ISNOT_UTF8
#define ISNOT_UTF8(_c_) (0 == IS_UTF8(_c_))
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
#define IS_DIR_SEP(_c_)  ((_c_) == DIR_SEP)
#endif

#ifndef IS_NOT_DIR_SEP
#define IS_NOT_DIR_SEP(_c_)  (0 == IS_DIR_SEP ((_c_)))
#endif

#ifndef IS_DIR_ABS
#define IS_DIR_ABS(_d_)  IS_DIR_SEP (*(_d_))
#endif

#ifndef IS_DIGIT
#define IS_DIGIT(_c_)  ('0' <= (_c_) && (_c_) <= '9')
#endif

#ifndef IS_CNTRL
#define IS_CNTRL(_c_)  (((_c_) < 0x20 && (_c_) >= 0) || (_c_) == 0x7f)
#endif

#ifndef IS_SPACE
#define IS_SPACE(_c_) ((_c_) == ' ' || (_c_) == '\t' || (_c_) == '\r' || (_c_) == '\n')
#endif

#ifndef IS_ALPHA
#define IS_ALPHA(_c_)  (((_c_) >= 'a' && (_c_) <= 'z') || ((_c_) >= 'A' && (_c_) <= 'Z'))
#endif

#ifndef IS_ALNUM
#define IS_ALNUM(_c_)  (IS_ALPHA((_c_)) || IS_DIGIT((_c_)))
#endif

#ifndef IS_HEX
#define IS_HEX(_c_)  ((IS_DIGIT((_c_)) || ((_c_) >= 'a' && (_c_) <= 'f') || ((_c_) >= 'A' && (_c_) <= 'F')))
#endif
