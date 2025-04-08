// provides: int rmdir_parents (const char *, int)
// requires: path/split.c
// requires: string/str_copy.c
// requires: string/bytelen.c
// requires: convert/format.c
// requires: dir/rmdir.c

int rmdir_parents (const char *dir, int verbose) {
  pathsplit_t *p = path_split (dir);
  if (NULL == p)
    return -1;

  size_t plen[p->num_items];

  for (int i = 0; i < p->num_items; i++)
    plen[i] = bytelen (p->items[i]);

  size_t dirlen = bytelen (dir);
  char path[dirlen + 1];

  size_t len = 0;

  for (int i = 0; i < p->num_items; i++) {
    str_copy (path + len, dirlen - len, p->items[i], plen[i]);
    len += plen[i];
    if (i < p->num_items - 1)
      path[len++] = DIR_SEP;

    path[len] = '\0';
  }

  int retval = 0;

  for (int i = p->num_items - 1; i >= 0; i--) {
    retval = sys_rmdir (path);

    if (-1 == retval) {
      if (verbose)
        tostderr ("rmdir: %s %s\n", path, errno_string (sys_errno));
      break;
    }

    if (verbose)
      tostdout ("removed directory: %s\n", path);

    len -= (plen[i] + 1);
    path[len] = '\0';
  }

  path_split_release (&p);
  return retval;
}

/* test {
// num-tests: 1
#define REQUIRE_RMDIR_PARENTS
#define REQUIRE_STR_EQ
#define REQUIRE_ATOI
#define REQUIRE_MAKE_DIR
#define REQUIRE_CHDIR
#define REQUIRE_FORMAT

#include <libc.h>

#define FUNNAME "rmdir_parents()"
#define MYTESTDIR "/tmp/rmdir_parents"

static int test_init (void) {
  if (-1 == make_dir_parents (MYTESTDIR "/aa/bb/cc", 0700)) return -1;
  return 0;
}

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  test_init ();

  const char *a =  "rmdir_parents/////aa//bb////cc///";
  sys_chdir ("/tmp");
  int retval = rmdir_parents (a, 0);

  if (-1 == retval)
    tostderr ("\e[31m[NOTOK]\e[m");
  else
    tostdout ("\e[32m[OK]\e[m\n");

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
