// as: path_concat
// provides: char *path_concat (const char *, size_t, const char *, size_t)
// requires: mem/talloc.c

char *path_concat (const char *a, size_t a_sz, const char *b, size_t b_sz) {
  char *spb = (char *) b;
  while (*spb == DIR_SEP) {
    spb++;
    b_sz--;
  }

  if (b_sz) {
    char *sp = (char *) a + (a_sz - 1);
    while (*sp == DIR_SEP) {
      sp--;
      a_sz--;
    }
  }

  char *new = Alloc (a_sz + b_sz + (b_sz != 0) + 1);
  *new = '\0';

  size_t i = 0;
  for (; i < a_sz; i++)
    new[i] = a[i];

  if (b_sz) {
    new[i++] = DIR_SEP;

    for (; i < a_sz + b_sz + 1; i++)
      new[i] = *spb++;
  } else {
    while (new[i - 1] == DIR_SEP)
      if (new[i - 2] == DIR_SEP)
        i--;
      else
        break;
  }

  new[i] = '\0';
  return new;
}

/* test {
// num-tests: 4
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
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;
  total++;
  if (second_test (total) == -1) failed++;
  total++;
  if (third_test (total) == -1) failed++;
  total++;
  if (fourth_test (total) == -1) failed++;

  return failed;
}
} */
