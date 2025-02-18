// provides: dirlist_t *dirlist (const char *)
// provides: void dirlist_release (dirlist_t **)
// requires: dir/is_directory.c
// requires: dir/dirwalk.c
// requires: dir/dirlist.h

// provides: dirlist_t *dirlist (const char *)
// provides: void dirlist_release (dirlist_t **)
// requires: stdlib/alloc.c
// requires: string/bytelen.c
// requires: string/str_new.c
// requires: dir/opendir.c
// requires: dir/dirent.h
// requires: dir/dirlist.h

dirlist_t *dirlist (const char *dir) {
  if (NULL == dir || 0 == is_directory (dir)) {
    sys_errno = EINVAL;
    return NULL;
  }

  dirwalk_t *dwalk = dirwalk_new (NULL, NULL, NULL);
  dwalk->depth = 1;
  int r = dirwalk_run (dwalk, dir);
  if (-1 == r)
    return NULL;

  return dwalk;
}

void dirlist_release (dirlist_t **dirp) {
  dirwalk_release (dirp);
}

/* test {
// num-tests: 1
#define REQUIRE_MAKE_DIR
#define REQUIRE_CHDIR
#define REQUIRE_OPEN
#define REQUIRE_RMDIR
#define REQUIRE_UNLINK
#define REQUIRE_FORMAT
#define REQUIRE_DIRLIST
#define REQUIRE_STR_EQ
#define REQUIRE_IS_DIRECTORY
#define REQUIRE_PATH_DIRNAME
#define REQUIRE_PATH_BASENAME
#define REQUIRE_ATOI

#include <libc.h>

#define DIRLIST_DIR "/tmp/dirlist_test"
#define DIRLIST_NUM_ENTRIES 8

static int test_init (void) {
  if (-1 == make_dir_parents (DIRLIST_DIR "/dir1", 0700)) return -1;
  if (-1 == make_dir_parents (DIRLIST_DIR "/dir2", 0700)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/dir1/aaaa", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/3", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/4", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/5", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (DIRLIST_DIR "/6", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  return 0;
}

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing dirlist() %s - ", total, __func__);

  if (-1 == test_init ()) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    return -1;
  }

  sys_chdir (DIRLIST_DIR);
  dirlist_t *dlist = dirlist (DIRLIST_DIR);

  if (dlist->num_entries != DIRLIST_NUM_ENTRIES) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  const char *entries[DIRLIST_NUM_ENTRIES] = {
    "dir1", "dir2", "1", "2", "3", "4", "5", "6"};

  for (int i = 0; i < DIRLIST_NUM_ENTRIES; i++) {
    char *entry = path_basename (dlist->entries[i]->bytes);
    int found = 0;
    for (int j = 0; j < DIRLIST_NUM_ENTRIES; j++) {
      if (str_eq (entry, entries[j])) {
        found = 1;
        break;
      }
    }

    if (0 == found) {
      tostderr ("\e[31m[NOTOK]\e[m\n");
      goto theend;
    }
  }

  for (int i = dlist->num_entries - 1; i >= 0; i--) {
    char *entry = dlist->entries[i]->bytes;
    if (is_directory (entry))
      sys_rmdir (entry);
    else
      sys_unlink (entry);
  }

  sys_chdir ("..");
  sys_rmdir (DIRLIST_DIR);

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  dirlist_release (&dlist);
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
