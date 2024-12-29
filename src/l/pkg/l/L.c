#define REQUIRE_MAP
#define REQUIRE_ALLOC
#define REQUIRE_STRING
#define REQUIRE_STDIO
#define REQUIRE_STR_COPY

#ifndef WITHOUT_STD_MODULE
#define REQUIRE_STR_EQ
#define REQUIRE_STR_EQ_N
#define REQUIRE_STR_CMP
#define REQUIRE_STR_NEW
#define REQUIRE_STR_STR
#define REQUIRE_QSORT
#define REQUIRE_STRTOL
#define REQUIRE_ATOI
#define REQUIRE_DECIMAL_TO_STRING
#define REQUIRE_READFILE
#endif

#ifndef WITHOUT_TERM_MODULE
#define REQUIRE_TERM
#define REQUIRE_TERM_GETKEY
#endif

#include <z/libc.h>
#include <z/l.h>

static l_T *__L__;
#define L __L__->self

#ifndef WITHOUT_STD_MODULE
#include "../modules/std-module.c"
#endif

#ifndef WITHOUT_TERM_MODULE
#include "../modules/term-module.c"
#endif

int main (int argc, char **argv) {
  mem_init (1 << 16);

  __L__ = __init_l__ ();

  int retval = L_OK;

  string *evalbuf = NULL;

  l_t *l = NULL;

  ifnot (sys_isatty (STDIN_FILENO)) { // we are in a pipe end
    evalbuf = string_new (256); // and there is no script to execute -
    int maxlen = 4095; // though there isn't a certainity, but we do our best for now

    for (;;) {
      char buf[maxlen + 1];
      int nbytes = sys_read (STDIN_FILENO, buf, maxlen);

      if (-1 == nbytes) goto theend;

      ifnot (nbytes) break;

      string_append_with_len (evalbuf, buf, nbytes);
    }
  }

  l = L.init_instance (__L__,
      LOpts(.argc = argc - 1, .argv = (const char **) (argv + 1)));

#ifndef WITHOUT_STD_MODULE
  __init_std_module__ (l);
#endif

#ifndef WITHOUT_TERM_MODULE
  __init_term_module__ (l);
#endif

  if (NULL == evalbuf) {
    if (1 == argc)
      return 1;

    retval = L.load_file (__L__, l, argv[1]);

  } else
    retval = L.eval_string (l, evalbuf->bytes);


theend:
  string_release (evalbuf);

//  mem_debug_all (1);
  __deinit_l__ (&__L__);

  mem_deinit ();

  return retval < 0 ? 1 : retval;
}
