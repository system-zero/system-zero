#ifndef C_H
#define C_H

/* This is my hble C (as c). Quite many of conveniences!
 * But also many i believe standardized by now macros.
 * Focusing mainly on an uncontrollable i'm afraid expressionism.
 * Compiles and obeys to the C11 standard, using definitions from
 * the newer POSIX and X/OPEN standards. 
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE   700

#include <stdint.h> /* get them (the integer types) right */
#include <stddef.h> /* for ptrdiff_t (see below the idx_t section) */
#include <stdlib.h> /* we need it here for malloc/free */
#include <string.h> /* probably we can do without it, if we work hard */
#include <errno.h>  /* this is a thread safe errno */

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

#ifndef muttable
#define muttable __attribute__((__weak__))
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

#ifndef unused
#define unused  __attribute__ ((unused))
#endif

/* for purity */
#ifndef bytelen
#define bytelen strlen
#endif
/* strlen might made sence, but now is confusing for young minds.
 * so dissasoiate it violently by forbiding it in the code.
 * Today a standard charlen() might justfied. */

typedef   signed int  utf8;
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

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
#define MAXLEN_LINE   4096
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
#define IS_DIR_SEP(c_)     (c_ == DIR_SEP)
#endif

#ifndef IS_NOT_DIR_SEP
#define IS_NOT_DIR_SEP(c_) (0 == IS_DIR_SEP (c_))
#endif

#ifndef IS_DIR_ABS
#define IS_DIR_ABS(d_)   IS_DIR_SEP (d_[0])
#endif

#ifndef IS_UTF8
#define IS_UTF8(c_)     (((c_) & 0xC0) == 0x80)
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
#define IS_HEX(c_)      (IS_DIGIT(c_) || (c_ >= 'a' && c_ <= 'f') || (c_ >= 'A' && c_ <= 'F')))
#endif

#ifndef ARRLEN
#define ARRLEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* start safe */
#define __ROOT_ERROR__         -10000
/* reserve some space
 * (maybe we map from EPERM (1) to ELOOP (40) at least, one day) */
#define __BASE_ERROR__         (__ROOT_ERROR__ - 200)
/* we want those two at least - for now */
#define INDEX_ERROR            (__BASE_ERROR__ - 1)
#define INTEGEROVERFLOW_ERROR  (__BASE_ERROR__ - 2)

#ifndef ALLOC_H
#define ALLOC_H

       /* Our alloc exits hard */
/* we set up an informative handler */
typedef void (*AllocErrorHandlerF) (int, size_t, char *, const char *, int);

AllocErrorHandlerF AllocErrorHandler;

/* perhaps one day */
#ifndef __REALLOC__
#define __REALLOC__ realloc
#endif

#ifndef __CALLOC__
#define __CALLOC__  calloc
#endif

/* reallocarray:
 * $OpenBSD: reallocarray.c,v 1.1 2014/05/08 21:43:49 deraadt Exp $
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net>
 */

/*         (Mon, Dec 28, 2000 at 20.58.59)
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
#define MEM_IS_INT_OVERFLOW(nmemb, ssize)                             \
 (((nmemb) >= MUL_NO_OVERFLOW || (ssize) >= MUL_NO_OVERFLOW) &&       \
  (nmemb) > 0 && SIZE_MAX / (nmemb) < (ssize))

#define Alloc(size) ({                                                \
  void *ptr__ = NULL;                                                 \
  if (MEM_IS_INT_OVERFLOW (1, (size))) {                              \
    errno = INTEGEROVERFLOW_ERROR;                                    \
    AllocErrorHandler (errno, (size),  __FILE__, __func__, __LINE__); \
  } else {                                                            \
    if (NULL == (ptr__ = __CALLOC__ (1, (size))))                     \
      AllocErrorHandler (errno, (size), __FILE__, __func__, __LINE__);\
    }                                                                 \
  ptr__;                                                              \
  })

#define Realloc(ptr, size) ({                                         \
  void *ptr__ = NULL;                                                 \
  if (MEM_IS_INT_OVERFLOW (1, (size))) {                              \
    errno = INTEGEROVERFLOW_ERROR;                                    \
    AllocErrorHandler (errno, (size),  __FILE__, __func__, __LINE__); \
  } else {                                                            \
    if (NULL == (ptr__ = __REALLOC__ ((ptr), (size))))                \
      AllocErrorHandler (errno, (size), __FILE__, __func__, __LINE__);\
    }                                                                 \
  ptr__;                                                              \
  })

/*
mutable public void __alloc_error_handler__ (int err, size_t size,
                           char *file, const char *func, int line) {
  fprintf (stderr, "MEMORY_ALLOCATION_ERROR\n");
  fprintf (stderr, "File: %s\nFunction: %s\nLine: %d\n", file, func, line);
  fprintf (stderr, "Size: %zd\n", size);

  if (err is INTEGEROVERFLOW_ERROR)
    fprintf (stderr, "Error: Integer Overflow Error\n");
  else
    fprintf (stderr, "Error: Not Enouch Memory\n");

  ... deinit stuff

  exit (1);
}
*/

#endif /* ALLOC_H */

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

/* be sure and clear them to make every body happy:
 * This is our main coding style/mechanism throughout our code universe */
#ifdef $my
#undef $my
#endif

#ifdef self
#undef self
#endif

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

#endif /* C_H */
