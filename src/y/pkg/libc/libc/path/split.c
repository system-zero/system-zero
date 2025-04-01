// as: path_split
// provides: pathsplit_t *path_split (const char *)
// provides: void path_split_release (pathsplit_t **)
// requires: string/str_new.c
// requires: string/str_byte_copy.c
// requires: mem/talloc.c
// requires: path/split.h

pathsplit_t *path_split (const char *path) {
  if (NULL == path || *path == '\0')
    return NULL;

  pathsplit_t *p = Alloc (sizeof (pathsplit_t));
  p->num_items = 0;
  p->num_alloc_items = 4;
  p->items = Alloc (sizeof (char *) * p->num_alloc_items);
  for (int i = 0; i < p->num_alloc_items; i++)
    p->items[i] = 0;

  const char *sp = path;
  const char *beg = sp;

  for (;;) {
    while (*sp == DIR_SEP) sp++;

    if (*sp == '\0')
      break;

    beg = sp;

    while (*sp != DIR_SEP) sp++;
    p->num_items++;

    if (p->num_items == p->num_alloc_items) {
      p->num_alloc_items *= 2;
      p->items = Realloc (p->items, sizeof (char *) * p->num_alloc_items);
    }

    p->items[p->num_items - 1] = str_new_with_len (beg, sp - beg);
  }

  return p;
}

void path_split_release (pathsplit_t **pp) {
  pathsplit_t *p = *pp;
  if (NULL == p)
    return;

  for (int i = 0; i < p->num_items; i++)
    Release (p->items[i]);

  Release (p->items);
  Release (p);
}

/* test {
// num-tests: 2
#define REQUIRE_PATH_SPLIT
#define REQUIRE_STR_EQ
#define REQUIRE_ATOI
#define REQUIRE_FORMAT

#include <libc.h>

#define FUNNAME "path_split()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *a = "///asdf/gkl/////ldld//";

  pathsplit_t *p = path_split (a);
  if (NULL == p) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  if (p->num_items != 3) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  int eq = str_eq (p->items[0], "asdf");

  if (eq == 0) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  eq = str_eq (p->items[1], "gkl");

  if (eq == 0) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  eq = str_eq (p->items[2], "ldld");

  if (eq == 0) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }
  tostdout ("\e[32m[OK]\e[m\n");

theend:
  path_split_release (&p);
  return eq == 1 ? 0 : -1;
}

static int second_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  int retval = -1;

  const char *a = "";
  pathsplit_t *p = path_split (a);

  if (NULL != p) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  a = "////";
  p = path_split (a);
  if (p->num_items) {
    tostderr ("\e[31m[NOTOK]\e[m");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  if (NULL != p)
    path_split_release (&p);

  return retval == -1 ? -1 : 0;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;
  total++;
  if (second_test (total) == -1) failed++;

  return failed;
}
} */
