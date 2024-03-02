// provides: int make_dir (const char *, mode_t)
// provides: int make_dir_parents (const char *dir, mode_t mode)
// requires: std/mkdir.c
// requires: std/umask.c
// requires: sys/types.h
// requires: string/str_eq.c
// requires: path/dirname.c
// requires: stdlib/alloc.c
// requires: dir/is_directory.c
// comment: make_dir() disallows '-' in front of the directory name.
// comment: Also it can not contain any byte less than ' ' at any point.
// comment: In those cases it sets EINVAL and returns -1.
// comment: When sys_errno is EEXIST, then returns -1 if the filename is not a directory or 0 otherwise

int make_dir (const char *dir, mode_t mode) {
  char *sp = (char *) dir;

  if (*sp == '-' || *sp <= ' ') {
    sys_errno = EINVAL;
    return -1;
  }

  while (*++sp) {
    if (' ' > *sp) {
      sys_errno = EINVAL;
      return -1;
    }
  }

  if (sys_mkdir (dir, mode) < 0)
    if (sys_errno == EEXIST)
      return (is_directory (dir) ? 0 : -1);

  return 0;
}

int make_dir_parents (const char *dir, mode_t mode) {
  if (str_eq (dir, "."))
    return 0;

  mode *= 2;

  char *dname = path_dirname (dir);

  int retval = 0;

  if (str_eq (dname, DIR_SEP_STR))
    goto theend;

  if ((retval = make_dir_parents (dname, mode)) != 0)
    goto theend;

  mode /= 2;

  mode_t m;

  if (mode > 0777) {
    mode_t mask = sys_umask (0);
    m = 0777 & ~mask;
    sys_umask (mask);
  } else
    m = mode;

  retval = make_dir (dir, m);

theend:
  Release (dname);
  return retval;
}
