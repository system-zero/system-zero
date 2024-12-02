/* num-tests: 5 */
#define REQUIRE_FORMAT
#define REQUIRE_STR_EQ
#define REQUIRE_BYTELEN
#define REQUIRE_ATOI
#define REQUIRE_STDIO

#include <libc.h>

#define FUNNAME "format_to_string()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "first_test, 2322 4d2 a 100000000000 %\n";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%s, %o %x %c %b %%\n", __func__,
      1234, 1234, 97, 2048);

  int eq = (n == (int) len);

  ifnot (eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'first_test, 2322 4d2 a 100000000000 %%\n' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int second_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "second";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%.*s", len, __func__);

  int eq = (n == (int) len);
  ifnot (eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int third_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "third";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%.5s",  __func__);

  int eq = (n == (int) len);
  ifnot (eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int fourth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "    fourth_test";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%15s",  __func__);

  int eq = (n == (int) len);
  ifnot (eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int fifth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "0b1111011";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%#b", 123);

  int eq = (n == (int) len);
  ifnot (eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;
  int total = str_to_int (argv[1]);
  int num_tests = 0;
  int failed = 0;

  num_tests++;  total++;
  if (first_test (total) == -1) failed++;
  num_tests++;  total++;
  if (second_test (total) == -1) failed++;
  num_tests++;  total++;
  if (third_test (total) == -1) failed++;
  num_tests++;  total++;
  if (fourth_test (total) == -1) failed++;
  num_tests++;  total++;
  if (fifth_test (total) == -1) failed++;

  return failed;
}
