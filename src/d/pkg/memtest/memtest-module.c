#define REQUIRE_STD_MODULE
#define REQUIRE_Z_ENV
#define REQUIRE_WRITE
#define REQUIRE_ALLOC
#define REQUIRE_STR_COPY
#define REQUIRE_STR_EQ
#define REQUIRE_BYTELEN
#define REQUIRE_DECIMAL_TO_STRING

//#define WITHOUT_FILE
//#include <stdio.h>

#define MEM_ZERO_FREED
#define MEM_ZERO_ON_REALLOC

#include <libc.h>

MODULE(memtest);

#define MAX_LEN_MSG 128
int failedTests = 0;
int okTests = 0;
int numTests = 0;
char msgStr[MAX_LEN_MSG];
decimal_t decString;

#define INTRO(__m__, __mlen__) do {                  \
  numTests++;                                        \
  char *nstr = int_to_string (&decString, numTests); \
  sys_write (1, "[", 1);                             \
  sys_write (1, nstr, decString.size);               \
  sys_write (1, "] ", 2);                            \
  sys_write (1, __m__, __mlen__);                    \
} while (0)

#define N_FUNC_TEST_TO_STR(__n__) ({                 \
  __n__++;                                           \
  size_t num = bytelen (__func__);                   \
  str_copy (msgStr, MAX_LEN_MSG, __func__, num);     \
  msgStr[num++] = ' ';                               \
  char *nstr = int_to_string (&decString, __n__);    \
  str_copy (msgStr + num, MAX_LEN_MSG - num, nstr, decString.size); \
  num += decString.size;                             \
  num;                                               \
})

#define SUMMARY() do {                               \
  char *nstr = int_to_string (&decString, numTests); \
  sys_write (1, "NumTests : [", 12);                 \
  sys_write (1, nstr, decString.size);               \
  sys_write (1, "]\n", 2);                           \
  nstr = int_to_string (&decString, failedTests);    \
  sys_write (1, "Failed : [", 10);                   \
  sys_write (1, nstr, decString.size);               \
  sys_write (1, "]\n", 2);                           \
  nstr = int_to_string (&decString, okTests);        \
  sys_write (1, "Passed : [", 10);                   \
  sys_write (1, nstr, decString.size);               \
  sys_write (1, "]\n", 2);                           \
} while (0)

static void test_str_eq (const char *a, const char *b, const char *msg, size_t len) {
  INTRO(msg, len);

  ifnot (str_eq (a, b)) {
    sys_write (2, ": failed\n", 9);
    failedTests++;
    return;
  }

  sys_write (1, ": ok\n", 5);
  okTests++;
}

static void test_int_eq (int a, int b, const char *msg, size_t len) {
  INTRO(msg, len);

  ifnot (a is b) {
    sys_write (2, ": failed\n", 9);
    failedTests++;
    return;
  }

  sys_write (1, ": ok\n", 5);
  okTests++;
}

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
  test_int_eq (ar is NULL, 0, msgStr, numbytes);

  ar[0] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[0] is NULL, 0, msgStr, numbytes);
  str_copy (ar[0], 7, "Abcdef", 6);

  ar[1] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[1] is NULL, 0, msgStr, numbytes);
  str_copy (ar[1], 7, "Ghjklm", 6);

  ar[2] = Alloc (7);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (ar[2] is NULL, 0, msgStr, numbytes);
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
  test_int_eq (s is NULL, 0, msgStr, numbytes);

  for (int i = 0; i < 14; i++)
    s[i] = i + 'a';
  s[14] = '\n';
  s[15] = '\0';

  s = Realloc (s, 42);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (s is NULL, 0, msgStr, numbytes);

  for (int i = 14; i < 30; i++)
    s[i] = i + ' ';
  s[30] = '\n';
  s[31] = '\0';

  test_release (s);

  s = Realloc (s, 240);
  numbytes = N_FUNC_TEST_TO_STR(n);
  test_int_eq (s is NULL, 0, msgStr, numbytes);

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
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Mem = {
      test : memtest
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_memtest_module__ (la_t *this) {
  (void) this;
}
