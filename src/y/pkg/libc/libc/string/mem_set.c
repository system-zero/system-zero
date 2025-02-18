// provides: void *mem_set (void *, int, size_t)
// provides: void *mem_set_s (void *, size_t, int, size_t)

void *mem_set (void *dest, int c, size_t count) {
  if (count == 0 || dest == NULL)
    return dest;

  unsigned char *ptr = dest;

  while (count--)
    *ptr++ = (unsigned char) c;

  return dest;
}

void *mem_set_s (void *dest, size_t destsz, int c, size_t count) {
  if (count == 0 || dest == NULL || 0 == destsz)
    return dest;

  unsigned char *ptr = dest;

  for (size_t i = 0; i < destsz && count--; i++)
    *ptr++ = (unsigned char) c;

  return dest;
}

/* test {
// num-tests: 1
#define REQUIRE_MEM_SET
#define REQUIRE_ATOI
#define REQUIRE_FORMAT
#define REQUIRE_ALLOC
#define REQUIRE_STR_EQ

#include <libc.h>
#define FUNNAME "mem_set()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  int retval = -1;

  char *a = Alloc (8);
  mem_set_s (a, 8, 0, 8);
  for (int i = 0; i < 8; i++) {
    if (a[i] != 0) {
      tostderr ("\e[31m[NOTOK]\e[m\n");
      goto theend;
    }
  }

  mem_set (a, 'x', 4);
  int eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (NULL, 8, 'c', 6);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 0, 'c', 6);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 8, 'c', 0);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 8, 'c', 6);

  eq = str_eq (a, "cccccc");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 1, 'x', 6);

  eq = str_eq (a, "xccccc");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 1;

theend:
  Release (a);
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
