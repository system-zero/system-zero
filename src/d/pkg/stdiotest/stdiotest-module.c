#define REQUIRE_STD_MODULE
#define REQUIRE_Z_ENV
#define REQUIRE_WRITE
#define REQUIRE_ALLOC
#define REQUIRE_STR_COPY
#define REQUIRE_STR_EQ
#define REQUIRE_BYTELEN
#define REQUIRE_VSNPRINTF
#define REQUIRE_TOUPPER
#define REQUIRE_DECIMAL_TO_STRING

#define REQUIRE_STDIO

#include <libc.h>

MODULE(stdiotest);

#include "../CommonTests/tests.c"

#define FNAME_1 "/tmp/first_fname"

static int fprintf_num_tests = 0;
static int fread_num_tests = 0;

static FILE *test_fopen_ok (const char *fname, const char *mode) {
  static int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  FILE *fp = sys_fopen (fname, mode);

  test_int_eq (fp is NULL, 0, msgStr, numbytes);
  return fp;
}

static void test_fclose_ok (FILE *fp) {
  static int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  int r = sys_fclose (fp);
  test_int_eq (r, 0, msgStr, numbytes);
}

static void first (void) {
  int numbytes = 0;

  FILE *fp = test_fopen_ok (FNAME_1, "w+");
  if (NULL is fp)
    exit_hard ("fatal fopen error", FOPEN_ERROR);

  numbytes = N_TEST_TO_STR("test_fprintf", fprintf_num_tests);
  int numwritten = sys_fprintf (fp, "%s", __func__);

  test_int_eq (numwritten, bytelen (__func__), msgStr, numbytes);

  test_fclose_ok (fp);

  fp = test_fopen_ok (FNAME_1, "r+");
  if (NULL is fp)
    exit_hard ("fatal fopen error", FOPEN_ERROR);

  char readbuf[numwritten + 1];
  size_t readbytes = sys_fread (readbuf, 1, numwritten, fp);
  readbuf[readbytes] = '\0';

  numbytes = N_TEST_TO_STR("test_fread", fread_num_tests);

  test_str_eq (readbuf, __func__, msgStr, numbytes);

  test_fclose_ok (fp);
}

static VALUE stdiotest (la_t *this) {
  (void) this;
  first ();
  SUMMARY();
  return INT(failedTests);
}

public int __init_stdiotest_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "stdiotest" , PTR(stdiotest), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Stdio = {
      test : stdiotest,
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_stdiotest_module__ (la_t *this) {
  (void) this;
}
