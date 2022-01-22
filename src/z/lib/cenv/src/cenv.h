#ifndef C_HDR
#define C_HDR

/* This is our humble C (as c) environment.
 * Quite many of conveniences!
 * Focusing mainly on an uncontrollable, i'm afraid, expressionism.
 * But also some, and standardized by now, macros.
 * Compiles and obeys to the C11 standard, using definitions from
 * the newer POSIX and X/OPEN standards. 
 */

#ifndef STD_POSIX_C_SOURCE_HDR
#define STD_POSIX_C_SOURCE_HDR
#define _POSIX_C_SOURCE 200809L
#endif

#ifndef STD_XOPEN_SOURCE_HDR
#define STD_XOPEN_SOURCE_HDR
#define _XOPEN_SOURCE   700
#endif

#ifdef REQUIRE_STD_DEFAULT_SOURCE
  #ifndef STD_DEFAULT_SOURCE_HDR
  #define STD_DEFAULT_SOURCE_HDR
  #define _DEFAULT_SOURCE
  #endif
#endif

#ifdef REQUIRE_STD_GNU_SOURCE
  #ifndef STD_GNU_SOURCE
  #define STD_GNU_SOURCE
  #define _GNU_SOURCE
  #endif
#endif

/* we need those to be funtional in this unit */
#include <stddef.h> /* for ptrdiff_t */
#include <stdint.h> /* integer types */
#include <stdlib.h> /* malloc/free */
#include <errno.h>

/* the following seven magnificent are unnegotiatable */
#ifndef OK
#define OK 0
#endif

#ifndef NOTOK
#define NOTOK -1
#endif

#ifndef ifnot
#define ifnot(__expr__) if (0 == (__expr__))
#endif

#ifndef is
#define is ==
#endif

#ifndef isnot
#define isnot !=
#endif

#ifndef or
#define or ||
#endif

#ifndef and
#define and &&
#endif

/* extreme (so negotiatable) */
#ifndef forever
#define forever for (;;)
#endif

/* likewise */
#ifndef loop
#define loop(__n__) for (int __i__ = 0; __i__ < __n__; __i__++)
#endif

#ifndef mutable
#define mutable __attribute__((__weak__))
#endif

/* i changed my mind about those two
 * (i do not (have to) use them anymore) */
#ifndef private
#define private __attribute__((visibility ("hidden")))
/* It doesn't work like static does, as it provides symbol collisions
 * on static targets, whereas "static" hides them.
 * (used to be used with -fvisibility=hidden in CFLAGS) */
#endif

#ifndef public
#define public __attribute__((visibility ("default")))
/* and that is what the compiler uses by default */
#endif

/* we used to use (void) arg;, so we've to modify the references */

#ifndef UNUSED
#define UNUSED  __attribute__ ((unused))
#endif

/* (not just) for purity */
/*
#ifndef bytelen
#define bytelen strlen
#endif
*/

/* strlen might made sence, but now is confusing for young minds.
 * so dissasociate it violently by forbiding it in the code.
 * Today a standard charlen() might justified. */

inline size_t bytelen (const char *str) {
  const char *sp = str;
  while (*sp) ++sp;
  return sp - str;
}
/* we don't really care about performance at this stage;
 * later when we go at the next computing level and we work with
 * such big strings:
 * https://medium.com/late-night-programming/strlen-buf-29eb94f8441f 
 * we'll do the best to understand the algorithm.
 * But for now, this subtracts clarity.
 * In fact for short strings <= 1000 bytes at least or even more, it is faster
 * than optimized libc strlen. Of course it is much slower on large byte arrays.
 */

typedef   signed int  utf8;
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

#ifndef SYSTEM_PATH
#define SYSTEM_PATH "/bin:/usr/local/bin:/usr/bin"
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096  /* bytes in a path name */
#endif

/* for consistency and fulfill expectations and be kind with
 * the human memory, we'll try to impose MAXLEN_* as prefix
 * to all those common macros */

#ifndef MAXLEN_PATH
#define MAXLEN_PATH   PATH_MAX
#endif

#ifndef MAXLEN_LINE
#define MAXLEN_LINE       4096
#endif

#ifndef MAXLEN_WORD
#define MAXLEN_WORD        256
#endif

#ifndef MAXLEN_ERR_MSG
#define MAXLEN_ERR_MSG     512
#endif

#ifndef MAXLEN_NAME
#define MAXLEN_NAME        16
#endif

#ifndef NAME_MAX
#define NAME_MAX 255  /* bytes in a file name */
#endif

#ifndef MAXLEN_PATTERN
#define MAXLEN_PATTERN 1024
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

/* in those coding cases, we want something really expressive */
#ifndef IS_NOT_DIR_SEP
#define IS_NOT_DIR_SEP(c_) (0 == IS_DIR_SEP (c_))
#endif

#ifndef IS_DIR_ABS
#define IS_DIR_ABS(d_)   IS_DIR_SEP (d_[0])
#endif

#ifndef IS_UTF8
#define IS_UTF8(c_)     (((c_) & 0xC0) == 0x80)
#endif

#ifndef ISNOT_UTF8
#define ISNOT_UTF8(c_) (0 == IS_UTF8(c_))
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

/* TODO: 
  #define PERM_PRIVATE  S_IRWXU
 */

#define REG_CHAR   '-'
#define BLK_CHAR   'b'
#define CHR_CHAR   'c'
#define DIR_CHAR   'd'
#define LNK_CHAR   'l'
#define FIFO_CHAR  'p'
#define SOCK_CHAR  's'
#define SUID_CHAR  's'
#define SGID_CHAR  'g'
#define SVTX_CHAR  't'

#ifndef ARRLEN
#define ARRLEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define __BASE_ERROR__         -10000
#define EINDEX                 (__BASE_ERROR__ - 1)
#define EINTEGEROVERFLOW       (__BASE_ERROR__ - 2)
#define ECANNOTGETCWD          (__BASE_ERROR__ - 3)

#ifndef ALLOC_HDR
#define ALLOC_HDR

       /* Our alloc exits hard */
/* we set up an informative handler */
typedef void (*AllocErrorHandlerF) (int, size_t, const char *, const char *, int);

AllocErrorHandlerF AllocErrorHandler;

/* perhaps one day */
#ifndef __REALLOC__
#define __REALLOC__ realloc
#endif

/* likewise */
#ifndef __CALLOC__
#define __CALLOC__  calloc
#endif

/* reallocarray:
 * $OpenBSD: reallocarray.c,v 1.1 2014/05/08 21:43:49 deraadt Exp $
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net>
 */

/*         (Mon, Dec 28, 2020 at 20.58.59)
 *  A year ago (notice the crazy coincidence)
 * Hi Paul,
 *  On Sat, Dec 28, at 10:28 Paul Eggert wrote:
 * ...
 * >> #define MUL_NO_OVERFLOW ((size_t) 1 << (sizeof (size_t) * 4))
 * >> #define MEM_IS_INT_OVERFLOW(nmemb, ssize)                       
 * >>       \
 * >> (((nmemb) >= MUL_NO_OVERFLOW || (ssize) >= MUL_NO_OVERFLOW) && 
 * >>       \
 * >> (nmemb) > 0 && SIZE_MAX / (nmemb) < (ssize))
 * > 
 * > Ouch. That code is not good. An unsigned division at runtime to do
 * > memory allocation? Gnulib does better than that already. Also,
 * > Glibc has some code in this area that we could migrate into Gnulib,
 * > that could be better yet.
 */

#define MUL_NO_OVERFLOW ((size_t) 1 << (sizeof (size_t) * 4))
#define MEM_IS_INT_OVERFLOW(nmemb_, ssize_)                            \
 (((nmemb_) >= MUL_NO_OVERFLOW || (ssize_) >= MUL_NO_OVERFLOW) &&      \
  (nmemb_) > 0 && SIZE_MAX / (nmemb_) < (ssize_))

#define Alloc(size_) ({                                                \
  void *ptr__ = NULL;                                                  \
  if (MEM_IS_INT_OVERFLOW (1, (size_))) {                              \
    errno = EINTEGEROVERFLOW;                                          \
    AllocErrorHandler (errno, (size_),  __FILE__, __func__, __LINE__); \
  } else {                                                             \
    if (NULL == (ptr__ = __CALLOC__ (1, (size_))))                     \
      AllocErrorHandler (errno, (size_), __FILE__, __func__, __LINE__);\
    }                                                                  \
  ptr__;                                                               \
  })

#define Realloc(ptr_, size_) ({                                        \
  void *ptr__ = NULL;                                                  \
  if (MEM_IS_INT_OVERFLOW (1, (size_))) {                              \
    errno = EINTEGEROVERFLOW;                                          \
    AllocErrorHandler (errno, (size_),  __FILE__, __func__, __LINE__); \
  } else {                                                             \
    if (NULL == (ptr__ = __REALLOC__ ((ptr_), (size_))))               \
      AllocErrorHandler (errno, (size_), __FILE__, __func__, __LINE__);\
    }                                                                  \
  ptr__;                                                               \
  })

/*
mutable public void __alloc_error_handler__ (int err, size_t size,
                           char *file, const char *func, int line) {
  fprintf (stderr, "MEMORY_ALLOCATION_ERROR\n");
  fprintf (stderr, "File: %s\nFunction: %s\nLine: %d\n", file, func, line);
  fprintf (stderr, "Size: %zd\n", size);

  if (err is EINTEGEROVERFLOW)
    fprintf (stderr, "Error: Integer Overflow Error\n");
  else
    fprintf (stderr, "Error: Not Enouch Memory\n");

  ... deinit stuff

  exit (1);
}
*/

#endif /* ALLOC_HDR */

#ifndef VA_ARGS_GET_FMT_STR
/* from man printf(3) Linux Programmer's Manual */
#define VA_ARGS_FMT_SIZE(fmt_)                                        \
({                                                                    \
  int size = 0;                                                       \
  va_list ap; va_start(ap, fmt_);                                     \
  size = vsnprintf (NULL, size, fmt_, ap);                            \
  va_end(ap);                                                         \
  size;                                                               \
})
/*
 * gcc used to complain on -Werror=alloc-size-larger-than= or -fsanitize=undefined,
 *  with:
 *  argument 1 range [18446744071562067968, 18446744073709551615]
 *  exceeds maximum object size 9223372036854775807
 *  in a call to built-in allocation function '__builtin_alloca_with_align'
 */
//#define VA_ARGS_FMT_SIZE (MAXLEN_LINE * 2)

#endif /* VA_ARGS_FMT_SIZE */

#ifndef VA_ARGS_GET_FMT_STR
#define VA_ARGS_GET_FMT_STR(buf_, size_, fmt_)                        \
({                                                                    \
  va_list ap; va_start(ap, fmt_);                                     \
  vsnprintf (buf_, size_ + 1, fmt_, ap);                              \
  va_end(ap);                                                         \
  buf_;                                                               \
})
#endif /* VA_ARGS_GET_FMT_STR */

#ifndef STR_FMT_WITH_LEN
#define STR_FMT_WITH_LEN(len_, fmt_, ...)                             \
({                                                                    \
  char buf_[len_];                                                    \
  snprintf (buf_, len_, fmt_, __VA_ARGS__);                           \
  buf_;                                                               \
})
#endif

#ifndef STR_FMT
#define STR_FMT(fmt_, ...)                                            \
({                                                                    \
  char buf_[MAXLEN_LINE];                                             \
  snprintf (buf_, MAXLEN_LINE, fmt_, __VA_ARGS__);                    \
  buf_;                                                               \
})
#endif

  /* idx_t */
/* 13:03 28 Dec Mon 2020
 * Date: Thu, 24 Dec 2020 12:16:58 -0300                              
 * From: Adhemerval Zanella <adhemerval.zanella@linaro.org>
 *   To: libc-alpha@sourceware.org, Paul Eggert <eggert@cs.ucla.edu>
 *   Cc: bug-gnulib@gnu.org
 * Subject: [GNULIB] [PATCH 2/5] Import idx.h from gnulib
 */

/* from gnulib/lib/idx.h
 * In the future, idx_t could be typedef'ed to a signed range type.
 * The clang "extended integer types", supported in Clang 11 or newer
 * <https://clang.llvm.org/docs/LanguageExtensions.html#extended-integer-types>,
 * are a special case of range types.  However, these types don't support binary
 * operators with plain integer types (e.g. expressions such as x > 1).
 * Therefore, they don't behave like signed types (and not like unsigned types
 * either).  So, we cannot use them here.
 */

/* Use the signed type 'ptrdiff_t'.
 * Note: ISO C does not mandate that 'size_t' and 'ptrdiff_t' have the same
 * size, but it is so on all platforms we have seen since 1990.
 */

typedef ptrdiff_t idx_t;

/* IDX_MAX is the maximum value of an idx_t.  */
#define IDX_MAX PTRDIFF_MAX

/* So far no need has been found for an IDX_WIDTH macro.
 * Perhaps there should be another macro IDX_VALUE_BITS that does
 * not count the sign bit and is therefore one less than
 * PTRDIFF_WIDTH.
 */

/* and last but not least a small prose */
#ifndef UNEXPRESSIONAL_C

/* really: we really really want to try to make C a little bit more
 * expressive, but without paying neither a bit of overhead.
 * We really really like C, and we know exactly why is written that
 * way, and we really really eppreciate this way. But we are common
 * human beings and we express differently than the machine. */
/*
 * Our humble mission, is to let C's programming mind flow expressed
 * naturally in code, just like writing some prose. We already used
 * the is|isnot|and|or,... macros, but we want a bit more freedom.
 * We especially want this in our codebase, much more than a usual
 * project, since this is meant also to serve as a documented system
 * description,  it is important to be declared clearly in code our
 * intention. I believe that making easier for anyone to understand
 * the intentions is crucial finally for the development, especially
 * for some like me, that do not write comments (for reasons). */
/*
 * So we'll do this with the C's only way, by using macros.
 * We know that macros are hiding details, so we'll not use them in
 * that way (at least in the beginning (Mon 28 Dec 2020)).
 * But let's start slow and see how it is going to be, by trying
 * first to bind libc functions, like isatty(). */
/*
 * Well.
 * We already have Types (structures with properties and methods),
 * which expressed like Type.method (type, data, ...).
 *
 * And we also introduced shell commands such: File.size filename
 * which maps exactly to the File.size (char *) library prototype.
 *
 * So by trying to be consistent we have some options.
 *
 *   FdIsATyy or
 *   Fd_is_a_tty or
 *   FdIsATerminal or
 *   FdReferToATerminal or
 *   Fd_Is_A_Terminal or
 *   FD_IS_A_TERMINAL
 */

/* Touch. Dots (.) are not allowed in C macros. Capitals are noisy.
 * So we have to use either underscore (_) or CamelCase, or a
 * combination. I'm tempted to use CamelCase and use the short
 * version, since it is less verbose, but i'm going to use the most
 * descriptive one as a start, mainly because it matches the
 * description of isatty() in the man page. */

 #define FdReferToATerminal(__fd__) isatty (__fd__)

 /* For this to work, we'll have to introduce a tool to get these
  * definitions from the sources, so we can always check for the
  * implementation in constant time. */

 /* But let the time to judge for the usefulness. In any case we
  * have to walk slowly and carefully that path. */

#endif /* UNEXPRESSIONAL_C */
/* We don't define anywhere in our codebase that macro, so they are
 * defined by default (but others might want them off). */

#endif /* C_HDR */

/* our environment */

/* Abstract as many details is possible here instead of doing this
 * in every compilation unit. If something will change in the future
 * it will be available instantly to the units.
 */

/* we use those at the very top of the compilation unit */
#ifdef REQUIRE_STDIO
  #ifndef STDIO_HDR
  #define STDIO_HDR
  #include <stdio.h>
  #endif /* STDIO_HDR */

#undef REQUIRE_STDIO
#endif /* REQUIRE_STDIO */

#ifdef REQUIRE_UNISTD
  #ifndef UNISTD_HDR
  #define UNISTD_HDR
  #include <unistd.h>
  #endif /* UNISTD_HDR */

#undef REQUIRE_UNISTD
#endif /* REQUIRE_UNISTD */

/* enable it because we need it here, but probably we can do without
 * it if we work hard, but define the macro anyway */
#define REQUIRE_STRING

#ifdef REQUIRE_STRING
  #ifndef STRING_HDR
  #define STRING_HDR
  #include <string.h>
  #endif /* STRING_HDR */

#undef REQUIRE_STRING
#endif /* REQUIRE_STRING */

#ifdef REQUIRE_STDARG
  #ifndef STDARG_HDR
  #define STDARG_HDR
  #include <stdarg.h>
  #endif /* STDARG_HDR */

#undef REQUIRE_STDARG
#endif /* REQUIRE_STDARG */

#ifdef REQUIRE_SIGNAL
  #ifndef SIGNAL_HDR
  #define SIGNAL_HDR
  #include <signal.h>
  #endif /* SIGNAL_HDR */

#undef REQUIRE_SIGNAL
#endif /* REQUIRE_SIGNAL */

#ifdef REQUIRE_STDBOOL
  #ifndef STDBOOL_HDR
  #define STDBOOL_HDR
    #define bool _Bool
    #define true 1
    #define false 0
  #endif /* STDBOOL_HDR */

#undef REQUIRE_STDBOOL
#endif /* REQUIRE_STDBOOL */

#ifdef REQUIRE_SCHED
  #ifndef SCHED_HDR
  #define SCHED_HDR
  #include <sched.h>
  #endif /* SCHED_HDR */

#undef REQUIRE_SCHED
#endif /* REQUIRE_SCHED */

#ifdef REQUIRE_LIMITS
  #ifndef LIMITS_HDR
  #define LIMITS_HDR
  #include <limits.h>
  #endif /* LIMITS_HDR */

#undef REQUIRE_LIMITS
#endif /* REQUIRE_LIMITS */

#ifdef REQUIRE_POLL
  #ifndef POLL_HDR
  #define POLL_HDR
  #include <poll.h>
  #endif /* POLL_HDR */

#undef REQUIRE_POLL
#endif /* REQUIRE_POLL */

#ifdef REQUIRE_LINUX_SCHED
  #ifndef LINUX_SCHED_HDR
  #define LINUX_SCHED_HDR
  #include <linux/sched.h>
  #endif /* LINUX_SCHED_HDR */

#undef REQUIRE_LINUX_SCHED
#endif /* REQUIRE_LINUX_SCHED */

#ifdef REQUIRE_SYSCALL
  #ifndef SYSCALL_HDR
  #define SYSCALL_HDR
  #include <syscall.h>
  #endif /* SYSCALL_HDR */

#undef REQUIRE_SYSCALL
#endif /* REQUIRE_SYSCALL */

#ifdef REQUIRE_SYSEXITS
  #ifndef SYSEXITS_HDR
  #define SYSEXITS_HDR
  #include <sysexits.h>
  #endif /* SYSEXITS_HDR */

#undef REQUIRE_SYSEXITS
#endif /* REQUIRE_SYSEXITS */

#ifdef REQUIRE_SYS_MOUNT
  #ifndef SYS_MOUNT_HDR
  #define SYS_MOUNT_HDR
  #include <sys/mount.h>
  #endif /* SYS_MOUNT_HDR */

#undef REQUIRE_SYS_MOUNT
#endif /* REQUIRE_SYS_MOUNT */

#ifdef REQUIRE_SYS_SIGNALFD
  #ifndef SYS_SIGNALFD_HDR
  #define SYS_SIGNALFD_HDR
  #include <sys/signalfd.h>
  #endif /* SYS_SIGNALFD_HDR */

#undef REQUIRE_SYS_SIGNALFD
#endif /* REQUIRE_SYS_SIGNALFD */

#ifdef REQUIRE_SYS_PRCTL
  #ifndef SYS_PRCTL_HDR
  #define SYS_PRCTL_HDR
  #include <sys/prctl.h>
  #endif /* SYS_PRCTL_HDR */

#undef REQUIRE_SYS_PRCTL
#endif /* REQUIRE_SYS_PRCTL */

#ifdef REQUIRE_SYS_WAIT
  #ifndef SYS_WAIT_HDR
  #define SYS_WAIT_HDR
  #include <sys/wait.h>
  #endif /* SYS_WAIT_HDR */

#undef REQUIRE_SYS_WAIT
#endif /* REQUIRE_SYS_WAIT */

#ifdef REQUIRE_SYS_MMAN
  #ifndef SYS_MMAN_HDR
  #define SYS_MMAN_HDR
  #include <sys/mman.h>
  #endif /* SYS_MMAN_HDR */

#undef REQUIRE_SYS_MMAN
#endif /* REQUIRE_SYS_MMAN */

#ifdef REQUIRE_SYS_SENDFILE
  #ifndef SYS_SENDFILE_HDR
  #define SYS_SENDFILE_HDR
  #include <sys/sendfile.h>
  #endif /* SYS_SENDFILE_HDR */

#undef REQUIRE_SYS_SENDFILE
#endif /* REQUIRE_SYS_SENDFILE */

#ifdef REQUIRE_SYS_STAT
  #ifndef SYS_STAT_HDR
  #define SYS_STAT_HDR
  #include <sys/stat.h>
  #endif /* SYS_STAT_HDR */

#undef REQUIRE_SYS_STAT
#endif /* REQUIRE_SYS_STAT */

#ifdef REQUIRE_SYS_PARAM
  #ifndef SYS_PARAM_HDR
  #define SYS_PARAM_HDR
  #include <sys/param.h>
  #endif /* SYS_PARAM_HDR */

#undef REQUIRE_SYS_PARAM
#endif /* REQUIRE_SYS_PARAM */

#ifdef REQUIRE_SYS_TYPES
  #ifndef SYS_TYPES_HDR
  #define SYS_TYPES_HDR
  #include <sys/types.h>
  #endif /* SYS_TYPES_HDR */

#undef REQUIRE_SYS_TYPES
#endif /* REQUIRE_SYS_TYPES */

#ifdef REQUIRE_SYS_SOCKET
  #ifndef SYS_SOCKET_HDR
  #define SYS_SOCKET_HDR
  #include <sys/socket.h>
  #endif /* SYS_SOCKET_HDR */

#undef REQUIRE_SYS_SOCKET
#endif /* REQUIRE_SYS_SOCKET */

#ifdef REQUIRE_SYS_SELECT
  #ifndef SYS_SELECT_HDR
  #define SYS_SELECT_HDR
  #include <sys/select.h>
  #endif /* SYS_SELECT_HDR */

#undef REQUIRE_SYS_SELECT
#endif /* REQUIRE_SYS_SELECT */

#ifdef REQUIRE_SYS_UN
  #ifndef SYS_UN_HDR
  #define SYS_UN_HDR
  #include <sys/un.h>
  #endif /* SYS_UN_HDR */

#undef REQUIRE_SYS_UN
#endif /* REQUIRE_SYS_UN */

#ifdef REQUIRE_SYS_UNAME
  #ifndef SYS_UNAME_HDR
  #define SYS_UNAME_HDR
  #include <sys/utsname.h>
  #endif /* SYS_UNAME_HDR */

#undef REQUIRE_SYS_UNAME
#endif /* REQUIRE_SYS_UNAME */

#ifdef REQUIRE_ARPA_INET
  #ifndef ARPA_INET_IN_HDR
  #define ARPA_INET_IN_HDR
  #include <arpa/inet.h>
  #endif /* ARPA_INET_HDR */

#undef REQUIRE_ARPA_INET
#endif /* REQUIRE_ARPA_INET */

#ifdef REQUIRE_NETINET_IN
  #ifndef NETINET_IN_HDR
  #define NETINET_IN_HDR
  #include <netinet/in.h>
  #endif /* NETINET_IN_HDR */

#undef REQUIRE_NETINET_IN
#endif /* REQUIRE_NETINET_IN */

#ifdef REQUIRE_NETDB
  #ifndef NETDB_HDR
  #define NETDB_HDR
  #include <netdb.h>
  #endif /* NETDB_HDR */

#undef REQUIRE_NETDB
#endif /* REQUIRE_NETDB */

#ifdef REQUIRE_TERMIOS
  #ifndef TERMIOS_HDR
  #define TERMIOS_HDR
  #include <termios.h>
  #endif /* TERMIOS_HDR */

#undef REQUIRE_TERMIOS
#endif /* REQUIRE_TERMIOS */

#ifdef REQUIRE_PTY
  #ifndef PTY_HDR
  #define PTY_HDR
  #include <pty.h>
  #endif /* PTY_HDR */

#undef REQUIRE_PTY
#endif /* REQUIRE_PTY */

#ifdef REQUIRE_DIRENT
  #ifndef DIRENT_HDR
  #define DIRENT_HDR
  #include <dirent.h>
  #endif /* DIRENT_HDR */

#undef REQUIRE_DIRENT
#endif /* REQUIRE_DIRENT */

#ifdef REQUIRE_FCNTL
  #ifndef FCNTL_HDR
  #define FCNTL_HDR
  #include <fcntl.h>
  #endif /* FCNTL_HDR */

#undef REQUIRE_FCNTL
#endif /* REQUIRE_FCNTL */

#ifdef REQUIRE_GLOB
  #ifndef GLOB_HDR
  #define GLOB_HDR
  #include <glob.h>
  #endif /* GLOB_HDR */

#undef REQUIRE_GLOB
#endif /* REQUIRE_GLOB */

#ifdef REQUIRE_TIME
  #ifndef TIME_HDR
  #define TIME_HDR
  #include <time.h>
  #endif /* TIME_HDR */

#undef REQUIRE_TIME
#endif /* REQUIRE_TIME */

#ifdef REQUIRE_MATH
  #ifndef MATH_HDR
  #define MATH_HDR
  #include <math.h>
  #endif /* MATH_HDR */

#undef REQUIRE_MATH
#endif /* REQUIRE_MATH */

#ifdef REQUIRE_LOCALE
  #ifndef LOCALE_HDR
  #define LOCALE_HDR
  #include <locale.h>
  #endif /* LOCALE_HDR */

#undef REQUIRE_LOCALE
#endif /* REQUIRE_LOCALE */

#ifdef REQUIRE_GRP
  #ifndef GRP_HDR
  #define GRP_HDR
  #include <grp.h>
  #endif /* GRP_HDR */

#undef REQUIRE_GRP
#endif /* REQUIRE_GRP */

#ifdef REQUIRE_PWD
  #ifndef PWD_HDR
  #define PWD_HDR
  #include <pwd.h>
  #endif /* PWD_HDR */

#undef REQUIRE_PWD
#endif /* REQUIRE_PWD */

#ifdef REQUIRE_PAM
  #ifndef PAM_HDR
  #define PAM_HDR
  #include <security/pam_appl.h>
  #include <security/pam_modules.h>
  #endif /* PAM_HDR */

#undef REQUIRE_PAM
#endif /* REQUIRE_PAM */

#ifdef REQUIRE_CTYPE
  #ifndef CTYPE_HDR
  #define CTYPE_HDR
  #include <ctype.h>
  #endif /* CTYPE_HDR */

#undef REQUIRE_CTYPE
#endif /* REQUIRE_CTYPE */

#ifdef REQUIRE_SYS_IOCTL
  #ifndef IOCTL_HDR
  #define IOCTL_HDR
  #include <sys/ioctl.h>
  #endif /* IOCTL_HDR */

#undef REQUIRE_SYS_IOCTL
#endif /* REQUIRE_SYS_IOCTL */

#ifdef REQUIRE_DLFCN
  #ifndef DLFCN_HDR
  #define DLFCN_HDR
  #include <dlfcn.h>
  #endif /* DLFCN_HDR */

#undef REQUIRE_DLFCN
#endif /* REQUIRE_DLFCN */

#ifndef DECLARE
#define DECLARE 1
#endif

/* modules */
#ifdef REQUIRE_STD_MODULE
  #ifndef REQUIRE_VMAP_TYPE
    #define REQUIRE_VMAP_TYPE    DECLARE
  #endif

  #ifndef REQUIRE_STRING_TYPE
    #define REQUIRE_STRING_TYPE  DECLARE
  #endif

  #ifndef REQUIRE_VSTRING_TYPE
    #define REQUIRE_VSTRING_TYPE DECLARE
  #endif

  #ifndef REQUIRE_LA_TYPE
    #define REQUIRE_LA_TYPE      DECLARE
  #endif

#undef REQUIRE_STD_MODULE
#endif

   /* forward declarations for -Wmissing declarations */
#define MODULE(_name_)                                      \
  public int __init_ ## _name_ ## _module__ (la_t *);       \
  public void __deinit_ ## _name_ ## _module__ (la_t *);

  /* this should be called inside of the __init_module__() */
#define __INIT_MODULE__(__l__)   \
do {                             \
  __INIT__(vmap);                \
  __INIT__(string);              \
  __INIT__(vstring);             \
  __LA__ = *la_get_root (__l__); \
} while (0)

  /* types */
#ifdef APPLICATION

  #ifndef APPLICATION_HDR
  #define APPLICATION_HDR

    #ifndef WITHOUT_STDARG
      #ifndef STDARG_HDR
      #define STDARG_HDR
      #include <stdarg.h>
      #endif
    #endif

    #ifndef WITHOUT_ARGPARSE
      #ifndef UNISTD_HDR
      #define UNISTD_HDR
      #include <unistd.h>
      #endif

      #ifndef WITHOUT_ARGPARSE_DECLARATION
        #define REQUIRE_ARGPARSE_TYPE DECLARE
      #else
        #define REQUIRE_ARGPARSE_TYPE DONOT_DECLARE
      #endif
    #endif

    #ifndef WITHOUT_IO
      #ifndef STDIO_HDR
      #define STDIO_HDR
      #include <stdio.h>
      #endif

      #ifndef WITHOUT_IO_DECLARATION
        #define REQUIRE_IO_TYPE DECLARE
      #else
        #define REQUIRE_IO_TYPE DONOT_DECLARE
      #endif
    #endif

  #endif /* APPLICATION_HDR */

  #ifndef WITHOUT_USAGE
    #ifdef APP_OPTS
    static const char *const usage[] = { \
      APPLICATION " " APP_OPTS,          \
    NULL,                                \
    };
    #endif
  #endif

#undef APPLICATION
#endif /* APPLICATION */


#ifdef REQUIRE_LIST_MACROS
  #ifndef LIST_MACROS_HDR
  #define LIST_MACROS_HDR
  #include <z/list.h>
  #endif /* LIST_MACROS_HDR */

#undef REQUIRE_LIST_MACROS
#endif /* REQUIRE_LIST_MACROS */

#ifdef REQUIRE_ERROR_TYPE
  #ifndef ERROR_TYPE_HDR
  #define ERROR_TYPE_HDR
  #include <z/error.h>
  #endif /* ERROR_TYPE_HDR */

  #if (REQUIRE_ERROR_TYPE == DECLARE)
  static  error_T errorType;
  #define Error   errorType.self
  #endif

#undef REQUIRE_ERROR_TYPE
#endif /* REQUIRE_ERROR_TYPE */

#ifdef REQUIRE_STRING_TYPE
  #ifndef STRING_TYPE_HDR
  #define STRING_TYPE_HDR
  #include <z/stringt.h>
  #endif /* STRING_TYPE_HDR */

  #if (REQUIRE_STRING_TYPE == DECLARE)
  static  string_T stringType;
  #define String   stringType.self
  #endif

  typedef string_t string;

#undef REQUIRE_STRING_TYPE
#endif /* REQUIRE_STRING_TYPE */

#ifdef REQUIRE_CSTRING_TYPE
  #ifndef CSTRING_TYPE_HDR
  #define CSTRING_TYPE_HDR
  #include <z/cstring.h>
  #endif /* CSTRING_TYPE_HDR */

  #if (REQUIRE_CSTRING_TYPE == DECLARE)
  static  cstring_T cstringType;
  #define Cstring   cstringType.self
  #endif

#undef REQUIRE_CSTRING_TYPE
#endif /* REQUIRE_CSTRING_TYPE */

#ifdef REQUIRE_VSTRING_TYPE
  #ifndef VSTRING_TYPE_HDR
  #define VSTRING_TYPE_HDR
  #include <z/vstring.h>
  #endif /* VSTRING_TYPE_HDR */

  #if (REQUIRE_VSTRING_TYPE == DECLARE)
  static  vstring_T vstringType;
  #define Vstring   vstringType.self
  #endif

#undef REQUIRE_VSTRING_TYPE
#endif /* REQUIRE_VSTRING_TYPE */

#ifdef REQUIRE_USTRING_TYPE
  #ifndef USTRING_TYPE_HDR
  #define USTRING_TYPE_HDR
  #include <z/ustring.h>
  #endif /* USTRING_TYPE_HDR */

  #if (REQUIRE_USTRING_TYPE == DECLARE)
  static  ustring_T ustringType;
  #define Ustring   ustringType.self
  #endif

#undef REQUIRE_USTRING_TYPE
#endif /* REQUIRE_USTRING_TYPE */

#ifdef REQUIRE_IO_TYPE
  #ifndef IO_TYPE_HDR
  #define IO_TYPE_HDR
  #include <z/io.h>
  #endif /* IO_TYPE_HDR */

  #if (REQUIRE_IO_TYPE == DECLARE)
  static  io_T   ioType;
  #define IO     ioType.self
  #define Stderr ioType.self.err
  #define Stdout ioType.self.out
  #define Input  ioType.self.input
  #define FD     ioType.self.fd

  #endif

#undef REQUIRE_IO_TYPE
#endif /* REQUIRE_IO_TYPE */

#ifdef REQUIRE_RLINE_TYPE
  #ifndef RLINE_TYPE_HDR
  #define RLINE_TYPE_HDR
  #include <z/rline.h>
  #endif /* RLINE_TYPE_HDR */

  #if (REQUIRE_RLINE_TYPE == DECLARE)
  static  rline_T rlineType;
  #define Rline   rlineType.self
  #endif

#undef REQUIRE_RLINE_TYPE
#endif /* REQUIRE_RLINE_TYPE */


#ifdef REQUIRE_JSON_TYPE
  #ifndef JSON_TYPE_HDR
  #define JSON_TYPE_HDR
  #include <z/json.h>
  #endif /* JSON_TYPE_HDR */

  #if (REQUIRE_JSON_TYPE == DECLARE)
  static  json_T jsonType;
  #define Json   jsonType.self
  #endif

#undef REQUIRE_JSON_TYPE
#endif /* REQUIRE_JSON_TYPE */

#ifdef REQUIRE_SH_TYPE
  #ifndef SH_TYPE_HDR
  #define SH_TYPE_HDR
  #include <z/sh.h>
  #endif /* SH_TYPE_HDR */

  #if (REQUIRE_SH_TYPE == DECLARE)
  static  sh_T shType;
  #define Sh   shType.self
  #endif

#undef REQUIRE_SH_TYPE
#endif /* REQUIRE_SH_TYPE */

#ifdef REQUIRE_OS_TYPE
  #ifndef OS_TYPE_HDR
  #define OS_TYPE_HDR
  #include <z/os.h>
  #endif /* OS_TYPE_HDR */

  #if (REQUIRE_OS_TYPE == DECLARE)
  static  os_T osType;
  #define OS   osType.self
  #endif

#undef REQUIRE_OS_TYPE
#endif /* REQUIRE_OS_TYPE */

#ifdef REQUIRE_FILE_TYPE
  #ifndef FILE_TYPE_HDR
  #define FILE_TYPE_HDR
  #include <z/file.h>
  #endif /* FILE_TYPE_HDR */

  #if (REQUIRE_FILE_TYPE == DECLARE)
  static  file_T fileType;
  #define File   fileType.self
  #endif

#undef REQUIRE_FILE_TYPE
#endif /* REQUIRE_FILE_TYPE */

#ifdef REQUIRE_PATH_TYPE
  #ifndef PATH_TYPE_HDR
  #define PATH_TYPE_HDR
  #include <z/path.h>
  #endif /* PATH_TYPE_HDR */

  #if (REQUIRE_PATH_TYPE == DECLARE)
  static  path_T pathType;
  #define Path   pathType.self
  #endif

#undef REQUIRE_PATH_TYPE
#endif /* REQUIRE_PATH_TYPE */

#ifdef REQUIRE_PROC_TYPE
  #ifndef PROC_TYPE_HDR
  #define PROC_TYPE_HDR
  #include <z/proc.h>
  #endif /* PROC_TYPE_HDR */

  #if (REQUIRE_PROC_TYPE == DECLARE)
  static  proc_T procType;
  #define Proc   procType.self
  #endif

#undef REQUIRE_PROC_TYPE
#endif /* REQUIRE_PROC_TYPE */

#ifdef REQUIRE_DIR_TYPE
  #ifndef DIR_TYPE_HDR
  #define DIR_TYPE_HDR

  #ifndef SYS_STAT_HDR
  #define SYS_STAT_HDR
  #include <sys/stat.h>
  #endif /* SYS_STAT_HDR */

  #include <z/dir.h>
  #endif /* DIR_TYPE_HDR */

  #if (REQUIRE_DIR_TYPE == DECLARE)
  static  dir_T dirType;
  #define Dir   dirType.self
  #endif

#undef REQUIRE_DIR_TYPE
#endif /* REQUIRE_DIR_TYPE */

#ifdef REQUIRE_TERM_TYPE
  #ifndef TERM_TYPE_HDR
  #define TERM_TYPE_HDR
  #include <z/term.h>
  #endif /* TERM_TYPE_HDR */

  #if (REQUIRE_TERM_TYPE == DECLARE)
  static  term_T termType;
  #define Term   termType.self
  #define Screen termType.self.screen
  #define Cursor termType.self.cursor
  #endif

#undef REQUIRE_TERM_TYPE
#endif /* REQUIRE_TERM_TYPE */

#ifdef REQUIRE_AUTH_TYPE
  #ifndef AUTH_TYPE_HDR
  #define AUTH_TYPE_HDR
  #include <z/auth.h>
  #endif /* AUTH_TYPE_HDR */

  #if (REQUIRE_AUTH_TYPE == DECLARE)
  static  auth_T authType;
  #define Auth   authType.self
  #endif

#undef REQUIRE_AUTH_TYPE
#endif /* REQUIRE_AUTH_TYPE */

#ifdef REQUIRE_I_TYPE
  #ifndef I_TYPE_HDR
  #define I_TYPE_HDR
  #include <z/i.h>
  #endif /* I_TYPE_HDR */

  #if (REQUIRE_I_TYPE == DECLARE)
  static  i_T iType;
  #define I   iType.self
  #endif

#undef REQUIRE_I_TYPE
#endif /* REQUIRE_I_TYPE */

#ifdef REQUIRE_IMAP_TYPE
  #ifndef IMAP_TYPE_HDR
  #define IMAP_TYPE_HDR
  #include <z/imap.h>
  #endif /* IMAP_TYPE_HDR */

  #if (REQUIRE_IMAP_TYPE == DECLARE)
  static  imap_T imapType;
  #define Imap   imapType.self
  #endif

#undef REQUIRE_IMAP_TYPE
#endif /* REQUIRE_IMAP_TYPE */

#ifdef REQUIRE_SMAP_TYPE
  #ifndef SMAP_TYPE_HDR
  #define SMAP_TYPE_HDR
  #include <z/smap.h>
  #endif /* SMAP_TYPE_HDR */

  #if (REQUIRE_SMAP_TYPE == DECLARE)
  static  smap_T smapType;
  #define Smap   smapType.self
  #endif

#undef REQUIRE_SMAP_TYPE
#endif /* REQUIRE_SMAP_TYPE */

#ifdef REQUIRE_VMAP_TYPE
  #ifndef VMAP_TYPE_HDR
  #define VMAP_TYPE_HDR
  #include <z/vmap.h>
  #endif /* VMAP_TYPE_HDR */

  #if (REQUIRE_VMAP_TYPE == DECLARE)
  static  vmap_T vmapType;
  #define Vmap   vmapType.self
  #endif

#undef REQUIRE_VMAP_TYPE
#endif /* REQUIRE_VMAP_TYPE */

#ifdef REQUIRE_ARGPARSE_TYPE
  #ifndef ARGPARSE_TYPE_HDR
  #define ARGPARSE_TYPE_HDR
  #include <z/argparse.h>
  #endif /* ARGPARSE_TYPE_HDR */

  #if (REQUIRE_ARGPARSE_TYPE == DECLARE)
    #ifndef Argparse
    static  argparse_T argparseType;
    #define Argparse   argparseType.self
    #endif
  #endif

#undef REQUIRE_ARGPARSE_TYPE
#endif /* REQUIRE_ARGPARSE_TYPE */

#ifdef REQUIRE_SYS_TYPE
  #ifndef SYS_TYPE_HDR
  #define SYS_TYPE_HDR
  #include <z/sys.h>
  #endif /* SYS_TYPE_HDR */

  #if (REQUIRE_SYS_TYPE == DECLARE)
  static  sys_T sysType;
  #define Sys   sysType.self
  #endif

#undef REQUIRE_SYS_TYPE
#endif /* REQUIRE_SYS_TYPE */

#ifdef REQUIRE_RE_TYPE
  #ifndef RE_TYPE_HDR
  #define RE_TYPE_HDR
  #include <z/re.h>
  #endif /* RE_TYPE_HDR */

  #if (REQUIRE_RE_TYPE == DECLARE)
  static  re_T reType;
  #define Re   reType.self
  #endif

#undef REQUIRE_RE_TYPE
#endif /* REQUIRE_RE_TYPE */

#ifdef REQUIRE_SPELL_TYPE
  #ifndef SPELL_TYPE_HDR
  #define SPELL_TYPE_HDR
  #include <z/spell.h>
  #endif /* SPELL_TYPE_HDR */

  #if (REQUIRE_SPELL_TYPE == DECLARE)
  static  spell_T spellType;
  #define Spell   spellType.self
  #endif

#undef REQUIRE_SPELL_TYPE
#endif /* REQUIRE_SPELL_TYPE */

#ifdef REQUIRE_VIDEO_TYPE
  #ifndef VIDEO_TYPE_HDR
  #define VIDEO_TYPE_HDR
  #include <z/video.h>
  #endif /* VIDEO_TYPE_HDR */

  #if (REQUIRE_VIDEO_TYPE == DECLARE)
  static  video_T videoType;
  #define Video   videoType.self
  #endif

#undef REQUIRE_VIDEO_TYPE
#endif /* REQUIRE_VIDEO_TYPE */

#ifdef REQUIRE_VUI_TYPE
  #ifndef VUI_TYPE_HDR
  #define VUI_TYPE_HDR
  #include <z/vui.h>
  #endif /* VUI_TYPE_HDR */

  #if (REQUIRE_VUI_TYPE == DECLARE)
  static  vui_T vuiType;
  #define Vui   vuiType.self
  #define Menu  vuiType.self.menu
  #endif

#undef REQUIRE_VUI_TYPE
#endif /* REQUIRE_VUI_TYPE */

#ifdef REQUIRE_READLINE_TYPE
  #ifndef READLINE_TYPE_HDR
  #define READLINE_TYPE_HDR
  #include <z/readline.h>
  #endif /* READLINE_TYPE_HDR */

  #if (REQUIRE_READLINE_TYPE == DECLARE)
  static  readline_T readlineType;
  #define Readline   readlineType.self
  #endif

#undef REQUIRE_READLINE_TYPE
#endif /* REQUIRE_READLINE_TYPE */

#ifdef REQUIRE_LA_TYPE
  #ifndef LA_TYPE_HDR
  #define LA_TYPE_HDR
  #include <z/la.h>
  #endif /* LA_TYPE_HDR */

  #if (REQUIRE_LA_TYPE == DECLARE)
  static  la_T __LA__;	
  #define La   __LA__.self
  #endif

#undef REQUIRE_LA_TYPE
#endif /* REQUIRE_LA_TYPE */

#ifdef REQUIRE_E_TYPE
  #ifndef E_TYPE_HDR
  #define E_TYPE_HDR
  #include <z/e.h>
  #endif /* E_TYPE_HDR */

  #if (REQUIRE_E_TYPE == DECLARE)
  typedef E_T      e_T;
  static  e_T   *__E__ = NULL;
  #define E      __E__->self
  #define Ed     __E__->__Ed__->self
  #define Win    __E__->__Ed__->__Win__.self
  #define Buf    __E__->__Ed__->__Buf__.self
  #define Msg    __E__->__Ed__->__Msg__.self
  #define EError __E__->__Ed__->__EError__.self
  #endif

#undef REQUIRE_E_TYPE
#endif /* REQUIRE_E_TYPE */

#ifdef REQUIRE_VWM_TYPE
  #ifndef VWM_TYPE_HDR
  #define VWM_TYPE_HDR
  #include <z/vwm.h>
  #endif /* VWM_TYPE_HDR */

  #if (REQUIRE_VWM_TYPE == DECLARE)
  static  vwm_T  *__VWM__ = NULL;
  #define Vwm     __VWM__->self
  #define Vframe  __VWM__->frame
  #define Vwin    __VWM__->win
  #endif

  #ifndef SHELL
  #define SHELL "zs"
  #endif

  #ifndef EDITOR
  #define EDITOR "E"
  #endif

  #ifndef DEFAULT_APP
  #define DEFAULT_APP SHELL
  #endif

#undef REQUIRE_VWM_TYPE
#endif /* REQUIRE_VWM_TYPE */

#ifdef REQUIRE_V_TYPE
  #ifndef V_TYPE_HDR
  #define V_TYPE_HDR
  #include <z/v.h>
  #endif /* V_TYPE_HDR */

  #if (REQUIRE_V_TYPE == DECLARE)
  static  v_t *__V__;
  #define V    __V__->self
  #endif

#undef REQUIRE_V_TYPE
#endif /* REQUIRE_V_TYPE */

#ifdef REQUIRE_MD5_TYPE
  #ifndef MD5_TYPE_HDR
  #define MD5_TYPE_HDR
  #include <z/md5.h>
  #endif /* MD5_TYPE_HDR */

  #if (REQUIRE_MD5_TYPE == DECLARE)
  static  md5_T   md5Type;
  #define Md5     md5Type.self
  #endif

#undef REQUIRE_MD5_TYPE
#endif /* REQUIRE_MD5_TYPE */

#ifdef REQUIRE_SHA256_TYPE
  #ifndef SHA256_TYPE_HDR
  #define SHA256_TYPE_HDR
  #include <z/sha256.h>
  #endif /* SHA256_TYPE_HDR */

  #if (REQUIRE_SHA256_TYPE == DECLARE)
  static  sha256_T   sha256Type;
  #define Sha256     sha256Type.self
  #endif

#undef REQUIRE_SHA256_TYPE
#endif /* REQUIRE_SHA256_TYPE */

#ifdef REQUIRE_SHA512_TYPE
  #ifndef SHA512_TYPE_HDR
  #define SHA512_TYPE_HDR
  #include <z/sha512.h>
  #endif /* SHA512_TYPE_HDR */

  #if (REQUIRE_SHA512_TYPE == DECLARE)
  static  sha512_T   sha512Type;
  #define Sha512     sha512Type.self
  #endif

#undef REQUIRE_SHA512_TYPE
#endif /* REQUIRE_SHA512_TYPE */

#ifdef REQUIRE_RANDOM_TYPE
  #ifndef RANDOM_TYPE_HDR
  #define RANDOM_TYPE_HDR
  #include <z/random.h>
  #endif /* RANDOM_TYPE_HDR */

  #if (REQUIRE_RANDOM_TYPE == DECLARE)
  static  random_T   randomType;
  #define Random     randomType.self
  #endif

#undef REQUIRE_RANDOM_TYPE
#endif /* REQUIRE_RANDOM_TYPE */

#ifdef REQUIRE_BCRYPT_TYPE
  #ifndef BCRYPT_TYPE_HDR
  #define BCRYPT_TYPE_HDR
  #include <z/bcrypt.h>
  #endif /* BCRYPT_TYPE_HDR */

  #if (REQUIRE_BCRYPT_TYPE == DECLARE)
  static  bcrypt_T   bcryptType;
  #define Bcrypt     bcryptType.self
  #endif

#undef REQUIRE_BCRYPT_TYPE
#endif /* REQUIRE_BCRYPT_TYPE */

#ifdef REQUIRE_BASE64_TYPE
  #ifndef BASE64_TYPE_HDR
  #define BASE64_TYPE_HDR
  #include <z/base64.h>
  #endif /* BASE64_TYPE_HDR */

  #if (REQUIRE_BASE64_TYPE == DECLARE)
  static  base64_T   base64Type;
  #define Base64     base64Type.self
  #endif

#undef REQUIRE_BASE64_TYPE
#endif /* REQUIRE_BASE64_TYPE */

#ifdef REQUIRE_CONTAIN_TYPE
  #ifndef CONTAIN_TYPE_HDR
  #define CONTAIN_TYPE_HDR
  #include <z/contain.h>
  #endif /* CONTAIN_TYPE_HDR */

  #if (REQUIRE_CONTAIN_TYPE == DECLARE)
  static  contain_T   containType;
  #define Contain     containType.self
  #endif

#undef REQUIRE_CONTAIN_TYPE
#endif /* REQUIRE_CONTAIN_TYPE */

// define it before net
#ifdef REQUIRE_URL_TYPE
  #ifndef URL_TYPE_HDR
  #define URL_TYPE_HDR
  #include <z/url.h>
  #endif /* URL_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_URL_TYPE
#endif /* REQUIRE_URL_TYPE */

#ifdef REQUIRE_DL_TYPE
  #ifndef DL_TYPE_HDR
  #define DL_TYPE_HDR
  #include <z/dl.h>
  #endif /* DL_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_DL_TYPE
#endif /* REQUIRE_DL_TYPE */

#ifdef REQUIRE_SUN_TYPE
  #ifndef SUN_TYPE_HDR
  #define SUN_TYPE_HDR
  #include <z/sun.h>
  #endif /* SUN_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_SUN_TYPE
#endif /* REQUIRE_SUN_TYPE */

#ifdef REQUIRE_MOON_TYPE
  #ifndef MOON_TYPE_HDR
  #define MOON_TYPE_HDR
  #include <z/moon.h>
  #endif /* MOON_TYPE_HDR */

  /* Do not define any type here. This unit is trying to be indepented. */

#undef REQUIRE_MOON_TYPE
#endif /* REQUIRE_MOON_TYPE */

#ifdef REQUIRE_NET_TYPE
  #ifndef NET_TYPE_HDR
  #define NET_TYPE_HDR
  #include <z/net.h>
  #endif /* NET_TYPE_HDR */

  #if (REQUIRE_NET_TYPE == DECLARE)
  static  net_T netType;
  #define Net   netType.self
  #endif

#undef REQUIRE_NET_TYPE
#endif /* REQUIRE_NET_TYPE */

/* Development (ignore this section) */

#ifdef REQUIRE_NETM_TYPE
  #ifndef NETM_TYPE_HDR
  #define NETM_TYPE_HDR
  #include <z/netm.h>
  #endif /* NETM_TYPE_HDR */

  #if (REQUIRE_NETM_TYPE == DECLARE)
  static  netm_T netmType;
  #define Netm   netmType.self
  #endif

#undef REQUIRE_NETM_TYPE
#endif /* REQUIRE_NETM_TYPE */

/* --------------------------------- */

#ifdef REQUIRE_KEYS_MACROS
  #ifndef KEYS_MACROS_HDR
  #define KEYS_MACROS_HDR

  #ifndef BACKSPACE_KEY
  #define BACKSPACE_KEY   010
  #endif

  #ifndef ESCAPE_KEY
  #define ESCAPE_KEY      033
  #endif

  #ifndef ARROW_DOWN_KEY
  #define ARROW_DOWN_KEY  0402
  #endif

  #ifndef ARROW_UP_KEY
  #define ARROW_UP_KEY    0403
  #endif

  #ifndef ARROW_LEFT_KEY
  #define ARROW_LEFT_KEY  0404
  #endif

  #ifndef ARROW_RIGHT_KEY
  #define ARROW_RIGHT_KEY 0405
  #endif

  #ifndef HOME_KEY
  #define HOME_KEY        0406
  #endif

  #ifndef FN_KEY
  #define FN_KEY(x)       (x + 0410)
  #endif

  #ifndef DELETE_KEY
  #define DELETE_KEY      0512
  #endif

  #ifndef INSERT_KEY
  #define INSERT_KEY      0513
  #endif

  #ifndef PAGE_DOWN_KEY
  #define PAGE_DOWN_KEY   0522
  #endif

  #ifndef PAGE_UP_KEY
  #define PAGE_UP_KEY     0523
  #endif

  #ifndef END_KEY
  #define END_KEY         0550
  #endif

  #ifndef CTRL
  #define CTRL(X) (X & 037)
  #endif

  #ifndef MODE_KEY
  #define MODE_KEY  CTRL('\\')
  #endif
  #endif /* KEYS_MACROS_HDR */

#undef REQUIRE_KEYS_MACROS
#endif /* REQUIRE_KEYS_MACROS */

#ifdef REQUIRE_TERM_MACROS
  #ifndef TERM_MACROS_HDR
  #define TERM_MACROS_HDR

  #define TERM_ITALIC                 "\033[3m"
  #define TERM_ITALIC_LEN             4
  #define TERM_INVERTED               "\033[7m"
  #define TERM_INVERTED_LEN           4
  #define TERM_LAST_RIGHT_CORNER      "\033[999C\033[999B"
  #define TERM_LAST_RIGHT_CORNER_LEN  12
  #define TERM_FIRST_LEFT_CORNER      "\033[H"
  #define TERM_FIRST_LEFT_CORNER_LEN  3
  #define TERM_GET_PTR_POS            "\033[6n"
  #define TERM_GET_PTR_POS_LEN        4
  #define TERM_SCREEN_SAVE            "\033[?47h"
  #define TERM_SCREEN_SAVE_LEN        6
  #define TERM_SCREEN_RESTORE        "\033[?47l"
  #define TERM_SCREEN_RESTORE_LEN     6
  #define TERM_SCREEN_CLEAR           "\033[2J"
  #define TERM_SCREEN_CLEAR_LEN       4
  #define TERM_SCROLL_RESET           "\033[r"
  #define TERM_SCROLL_RESET_LEN       3
  #define TERM_GOTO_PTR_POS_FMT       "\033[%d;%dH"
  #define TERM_CURSOR_HIDE            "\033[?25l"
  #define TERM_CURSOR_HIDE_LEN        6
  #define TERM_CURSOR_SHOW            "\033[?25h"
  #define TERM_CURSOR_SHOW_LEN        6
  #define TERM_CURSOR_RESTORE         "\0338"
  #define TERM_CURSOR_RESTORE_LEN     2
  #define TERM_CURSOR_SAVE            "\0337"
  #define TERM_CURSOR_SAVE_LEN        2
  #define TERM_AUTOWRAP_ON            "\033[?7h"
  #define TERM_AUTOWRAP_ON_LEN        5
  #define TERM_AUTOWRAP_OFF           "\033[?7l"
  #define TERM_AUTOWRAP_OFF_LEN       5
  #define TERM_BELL                   "\033[7"
  #define TERM_BELL_LEN               3
  #define TERM_NEXT_BOL              "\033E"
  #define TERM_NEXT_BOL_LEN           2
  #define TERM_SCROLL_REGION_FMT      "\033[%d;%dr"
  #define TERM_COLOR_RESET            "\033[m"
  #define TERM_COLOR_RESET_LEN        3
  #define TERM_SET_COLOR_FMT          "\033[%dm"
  #define TERM_SET_COLOR_FMT_LEN      5
  #define TERM_LINE_CLR_EOL           "\033[2K"
  #define TERM_LINE_CLR_EOL_LEN       4

  #define COLOR_RED         31
  #define COLOR_GREEN       32
  #define COLOR_YELLOW      33
  #define COLOR_BLUE        34
  #define COLOR_MAGENTA     35
  #define COLOR_CYAN        36
  #define COLOR_WHITE       37
  #define COLOR_FG_NORMAL   39
  #define COLOR_BG_NORMAL   49

  #define COLOR_BOX         COLOR_YELLOW

  #define TERM_MAKE_COLOR(clr) \
  ({char b__[8];snprintf (b__, 8, TERM_SET_COLOR_FMT, (clr));b__;})
  #define TERM_SEND_ESC_SEQ(seq_) IO.fd.write (this->out_fd, seq_, seq_ ## _LEN)
  #define SEND_ESC_SEQ(fd_, seq_) IO.fd.write ((fd_), seq_, seq_ ## _LEN)

  #define COLOR_MENU_HEADER COLOR_CYAN
  #define COLOR_MENU_BG     COLOR_RED
  #define COLOR_MENU_SEL    COLOR_GREEN

  #define COLOR_SU          COLOR_RED
  #define COLOR_BOX         COLOR_YELLOW
  #define COLOR_MSG         COLOR_YELLOW
  #define COLOR_ERROR       COLOR_RED
  #define COLOR_PROMPT      COLOR_YELLOW
  #define COLOR_NORMAL      COLOR_FG_NORMAL
  #define COLOR_TOPLINE     COLOR_YELLOW
  #define COLOR_DIVIDER     COLOR_MAGENTA
  #define COLOR_WARNING     COLOR_MAGENTA
  #define COLOR_SUCCESS     COLOR_GREEN
  #define COLOR_STATUSLINE  COLOR_BLUE
  #endif /* TERM_MACROS_HDR */

  #define TERM_DONOT_SAVE_SCREEN    (1 << 0)
  #define TERM_DONOT_CLEAR_SCREEN   (1 << 1)
  #define TERM_DONOT_RESTORE_SCREEN (1 << 2)

#undef TERM_MACROS
#endif /*TERM_MACROS */

#ifdef REQUIRE_MAP_MACROS
  #ifndef MAP_MACROS_HDR
  #define MAP_MACROS_HDR

  #define MAP_DEFAULT_LENGTH 32
  // hs_ = ((hs_ << 5) + hs_) + __key__[i_++];              
  #define MAP_HASH_KEY(__map__, __key__) ({                   \
    ssize_t hs_ = 5381; int i_ = 0;                           \
    while (__key__[i_])                                       \
      hs_ = (hs_ * 33) ^ __key__[i_++];                       \
    hs_ % __map__->num_slots;                                 \
  })

  #define MAP_RELEASE_SLOT(_it, _tp, _fun)                    \
  ({                                                          \
    while (_it) {                                             \
      _tp *_tmp = _it->next;                                  \
      free (_it->key);                                        \
      _fun (_it->value);                                      \
      free (_it);                                             \
      _it = _tmp;                                             \
    }                                                         \
  })

  #define MAP_CLEAR(_map, _fun)                               \
  ({                                                          \
    for (size_t i_ = 0; i_ < _map->num_slots; i_++) {         \
      ifnot (_map->slots[i_]) continue;                       \
      _fun (_map->slots[i_]);                                 \
      _map->slots[i_] = NULL;                                 \
    }                                                         \
    _map->num_keys = 0;                                       \
  })

  #define MAP_RELEASE(_map, _fun)                             \
  do {                                                        \
    if (_map is NULL) return;                                 \
    _fun (_map);                                              \
    free (_map->slots);                                       \
    free (_map);                                              \
    _map = NULL;                                              \
  } while (0)

  #define MAP_NEW(_TP, _tp, _num)                             \
  ({                                                          \
     _TP *_map = Alloc (sizeof (_TP));                        \
     int _num_slots = (_num < 1 ? MAP_DEFAULT_LENGTH : _num); \
     _map->slots = Alloc (sizeof (_tp *) * num_slots);        \
     _map->num_slots = _num_slots;                            \
     _map->num_keys = 0;                                      \
     for (;--_num_slots >= 0;) _map->slots[_num_slots] = NULL;\
     _map;                                                    \
  })

  #define MAP_GET(_tp, _map, _key, _idx)        \
  ({                                            \
    _idx = MAP_HASH_KEY (_map, _key);           \
    _tp *_slot = _map->slots[_idx];             \
    while (_slot) {                             \
      if (Cstring.eq (_slot->key, _key)) break; \
      _slot = _slot->next;                      \
    }                                           \
    _slot;                                      \
  })

  #define MAP_POP(_tp, _map, _key, _idx)        \
  ({                                            \
    _idx = MAP_HASH_KEY (_map, _key);           \
    _tp *_slot = _map->slots[_idx];             \
    _tp *_prev = _slot;                         \
    while (_slot) {                             \
      if (Cstring.eq (_slot->key, _key)) {      \
        _prev->next = _slot->next;              \
        break;                                  \
      }                                         \
      _prev = _slot;                            \
      _slot = _slot->next;                      \
    }                                           \
    if (_slot != NULL) {                        \
      _map->num_keys--;                         \
      if (_prev == _slot)                       \
        _map->slots[_idx] = _slot->next;        \
    }                                           \
    _slot;                                      \
  })

  #define MAP_SET(_tp_, _map_, _key_, _val_)            \
  ({                                                    \
    uint _idx_ = 0;                                     \
    _tp_ *_it_ = MAP_GET(_tp_, _map_, _key_, _idx_);    \
    ifnot (NULL is _it_) {                              \
      _it_->value = _val_;                              \
    } else {                                            \
      _it_ = Alloc (sizeof (_tp_));                     \
      _it_->key = Cstring.dup (_key_, bytelen (_key_)); \
      _it_->value = _val_;                              \
      _it_->next = _map_->slots[_idx_];                 \
      _map_->slots[_idx_] = _it_;                       \
      _map_->num_keys++;                                \
    }                                                   \
    _it_;                                               \
  })

  #define MAP_NEW_ITEM(_tp_, _map_, _key_)              \
  ({                                                    \
    uint _idx_ = 0;                                     \
    _tp_ *_it_ = MAP_GET(_tp_, _map_, _key_, _idx_);    \
    if (NULL is _it_) {                                 \
      _it_ = Alloc (sizeof (_tp_));                     \
      _it_->key = Cstring.dup (_key_, bytelen (_key_)); \
      _it_->next = _map_->slots[_idx_];                 \
      _map_->slots[_idx_] = _it_;                       \
      _map_->num_keys++;                                \
    }                                                   \
    _it_;                                               \
  })

  #endif /* MAP_MACROS_HDR */

#undef REQUIRE_MAP_MACROS
#endif /* REQUIRE_MAP_MACROS */

#ifdef LIBRARY

  #ifndef LIBRARY_HDR
  #define LIBRARY_HDR
  #endif /* LIBRARY_HDR */

#undef LIBRARY
#endif /* LIBRARY */

#ifndef OPT_NO_VERBOSE
#define OPT_NO_VERBOSE 0
#endif

#ifndef OPT_VERBOSE_ON_ERROR
#define OPT_VERBOSE_ON_ERROR 1
#endif

#ifndef OPT_VERBOSE
#define OPT_VERBOSE 2
#endif

#ifndef OPT_VERBOSE_EXTRA
#define OPT_VERBOSE_EXTRA 3
#endif

#ifndef OPT_NO_FORCE
#define OPT_NO_FORCE       0
#endif

#ifndef OPT_FORCE
#define OPT_FORCE          1
#endif

#ifndef OPT_NO_BACKUP
#define OPT_NO_BACKUP      0
#endif

#ifndef OPT_BACKUP
#define OPT_BACKUP         1
#endif

#ifndef OPT_NO_DEREFERENCE
#define OPT_NO_DEREFERENCE 0
#endif

#ifndef OPT_DEREFERENCE
#define OPT_DEREFERENCE    1
#endif

#ifndef OPT_NO_PRESERVE
#define OPT_NO_PRESERVE    0
#endif

#ifndef OPT_PRESERVE
#define OPT_PRESERVE       1
#endif

#ifndef OPT_PRESERVE_OWNER
#define OPT_PRESERVE_OWNER 2
#endif

#ifndef OPT_NO_RECURSIVE
#define OPT_NO_RECURSIVE   0
#endif

#ifndef OPT_RECURSIVE
#define OPT_RECURSIVE      1
#endif

#ifndef OPT_NO_UPDATE
#define OPT_NO_UPDATE      0
#endif

#ifndef OPT_UPDATE
#define OPT_UPDATE         1
#endif

#ifndef OPT_NO_ALL
#define OPT_NO_ALL         0
#endif

#ifndef OPT_ALL
#define OPT_ALL            1
#endif

#ifndef OPT_NO_INTERACTIVE
#define OPT_NO_INTERACTIVE 0
#endif

#ifndef OPT_INTERACTIVE
#define OPT_INTERACTIVE    1
#endif

#ifndef OPT_MAXDEPTH
#define OPT_MAXDEPTH 1024
#endif

#ifndef OPT_NO_YEAR
#define OPT_NO_YEAR -1
#endif

#ifndef OPT_NO_MONTH
#define OPT_NO_MONTH -1
#endif

#ifndef OPT_NO_DAY
#define OPT_NO_DAY -1
#endif

/* Those application routines are quite the same and is useless
 * to repeat ourselves.
 */
#define __INIT__(_T_) _T_ ## Type = __init_ ## _T_ ## __ ()

#define __INIT_APP__                 \
  __INIT__ (argparse);               \
  __INIT__ (io);                     \
  int version = 0;                   \
  int retval  = 0;                   \
  char *progname = (char *) argv[0]; \
  int argparse_flags = 0;            \
  (void) progname;                   \
  argparse_t argparser

#define PARSE_ARGS                                               \
  if (0 == isatty (STDIN_FILENO))                                \
    argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;            \
  Argparse.init (&argparser, options, usage, argparse_flags);    \
  argc = Argparse.exec (&argparser, argc, (const char **) argv); \
  CHECK_VERSION

#define CHECK_VERSION  \
  if (version) {       \
    fprintf (stderr, "%s\n", VERSION_STRING); \
    return 1;          \
  }

#define CHECK_ARGC     \
  ifnot (argc) {       \
    Argparse.print_usage (&argparser); \
    return 1;          \
  }

/* ----------------------------------------------- */
/* make everybody happy:
 * This is our main coding style/mechanism throughout our
 * code universe */
#ifdef $my
#undef $my
#endif

#ifdef self
#undef self
#endif

