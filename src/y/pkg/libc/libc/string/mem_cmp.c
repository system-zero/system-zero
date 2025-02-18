// provides: int mem_cmp (const void *, const void *, int)

int mem_cmp (const void *lhs, const void *rhs, int n) {
  if (lhs == rhs) return 0;

  const uchar *s1 = lhs;
  const uchar *s2 = rhs;

  for (;n && *s1 == *s2; n--, s1++, s2++);

  return n ? *s1 - *s2 : 0;
}

/* test {
// num-tests: 1

#define REQUIRE_MEM_CMP
#define REQUIRE_ATOI
#define REQUIRE_FORMAT
#define REQUIRE_ALLOC
#define REQUIRE_STR_COPY

#include <libc.h>
#define FUNNAME "mem_cmp()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  int retval = -1;

  char *a = Calloc (8);
  str_copy (a, 8, "asdf", 4);
  a[5] = 'h';

  char *b = Calloc (8);
  str_copy (b, 8, "asd", 3);

  char *c = Calloc (8);
  str_copy (c, 8, "asdf", 4);
  c[5] = 'h';
  c[6] = 'j';

  int r = mem_cmp (a, a, 1);

  if (r != 0) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  r = mem_cmp (a, b, 3);

  if (r != 0) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  r = mem_cmp (a, b, 4);

  if (r < 0) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  r = mem_cmp (a, c, 6);

  if (r != 0) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  r = mem_cmp (a, c, 7);

  if (r > 0) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 1;

theend:
  Release (a);
  Release (b);
  Release (c);

  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;

  return failed;
}
} */
