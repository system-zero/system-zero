#define REQUIRE_WRITE
#define REQUIRE_ALLOC
#define REQUIRE_STR_COPY
#define REQUIRE_STR_EQ
#define REQUIRE_BYTELEN
#define REQUIRE_VSNPRINTF
#define REQUIRE_TOUPPER
#define REQUIRE_DECIMAL_TO_STRING
#define MEM_ZERO_FREED

#define REQUIRE_MODULE_COMPAT
#include <libc.h>

MODULE(memtest);

#include "../CommonTests/tests.c"

static void test_init () {
  int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (MemInit (1024), 1, msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (MemInit (1024), 0, msgStr, numbytes);
}

static void test_deinit () {
  static int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (MemDeinit (), 0, msgStr, numbytes);
}

static void test_release (void *s) {
  static int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (Release (s), 0, msgStr, numbytes);
}

static void test_arrays (void) {
  int n = 0;
  int numbytes;

  size_t arlen = 3;
  char **ar = Alloc (sizeof (char *) * arlen);

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar == NULL, 0, msgStr, numbytes);

  ar[0] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[0] == NULL, 0, msgStr, numbytes);
  str_copy (ar[0], 7, "Abcdef", 6);

  ar[1] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[1] == NULL, 0, msgStr, numbytes);
  str_copy (ar[1], 7, "Ghjklm", 6);

  ar[2] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[2] == NULL, 0, msgStr, numbytes);
  str_copy (ar[2], 7, "Nopqrs", 6);

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (ar[0], "Abcdef", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (ar[1], "Ghjklm", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (ar[2], "Nopqrs", msgStr, numbytes);

  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*ar,       "Abcdef", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*(ar + 1), "Ghjklm", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*(ar + 2), "Nopqrs", msgStr, numbytes);

  char **t = ar;
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*t++, "Abcdef", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*t++, "Ghjklm", msgStr, numbytes);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_str_eq (*t++, "Nopqrs", msgStr, numbytes);

  for (size_t i = 0; i < arlen; i++)
    test_release (ar[i]);

  test_release (ar);
}

static void test_strings (void) {
  int n = 0;
  int numbytes;

  numbytes = N_FUNC_TEST_TO_STR(n);
  char *s = Alloc (130);
  test_int_eq (s == NULL, 0, msgStr, numbytes);

  for (int i = 0; i < 14; i++)
    s[i] = i + 'a';
  s[14] = '\n';
  s[15] = '\0';

  s = Realloc (s, 42);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (s == NULL, 0, msgStr, numbytes);

  for (int i = 14; i < 30; i++)
    s[i] = i + ' ';
  s[30] = '\n';
  s[31] = '\0';

  test_release (s);

  s = Realloc (s, 240);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (s == NULL, 0, msgStr, numbytes);

  for (int i = 0; i < 30; i++)
    s[i] = 'a';

  for (int i = 30; i < 38; i++)
    s[i] = (i - 30) + 'A';
  s[36] = '\n';
  s[37] = '\0';

  test_release (s);

  test_deinit();
}

static VALUE memtest (la_t *this) {
  (void) this;

  test_init ();
  test_strings ();
  test_arrays ();
  test_deinit ();

  SUMMARY();

  int r = failedTests;

  failedTests = 0;
  okTests     = 0;
  numTests    = 0;

  return INT(r);
}

public int __init_memtest_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "memtest" , PTR(memtest), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK != (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Mem = {
      test : memtest
     }
   );

  err = La.eval_string (this, evalString);
  if (err != LA_OK) return err;
  return LA_OK;
}

public void __deinit_memtest_module__ (la_t *this) {
  (void) this;
}
