/* num-tests: 4 */
#define REQUIRE_PATH_CONCAT
#define REQUIRE_STR_EQ
#define REQUIRE_ATOI
#define REQUIRE_STDIO

#include <libc.h>

#define FUNNAME "path_concat()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *a = "asdf/gkl/";
  const char *b = "/qwert";
  char *p = path_concat (a, 9, b, 6);
  int eq = str_eq (p, "asdf/gkl/qwert");
  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf/gkl/qwert' found %s\n", p);

  Release (p);
  return eq == 1 ? 0 : -1;
}

static int second_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *a = "asdf/gkl///";
  const char *b = "";
  char *p = path_concat (a, 11, b, 0);
  int eq = str_eq (p, "asdf/gkl/");

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf/gkl/' found %s\n", p);

  Release (p);

  return eq == 1 ? 0 : -1;
}

static int third_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *a = "asdf/gkl///";
  const char *b = "//";
  char *p = path_concat (a, 11, b, 2);
  int eq = str_eq (p, "asdf/gkl/");

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf/gkl/' found %s\n", p);

  Release (p);

  return eq == 1 ? 0 : -1;
}

static int fourth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *a = "asdf/gkl///";
  const char *b = "//qwert";
  char *p = path_concat (a, 11, b, 7);
  int eq = str_eq (p, "asdf/gkl/qwert");

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf/gkl/qwert' found %s\n", p);

  Release (p);

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

  return failed;
}
