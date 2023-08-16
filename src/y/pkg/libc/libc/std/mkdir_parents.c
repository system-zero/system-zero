// provides: long sys_mkdir_parents (const char *, mode_t)
// requires: unistd/fcntl.h
// requires: sys/types.h
// requires: string/str_eq.c
// requires: file/exists.c
// requires: dir/is_directory.c
// requires: path/dirname.c
// requires: std/umask.c
// requires: stdlib/alloc.c

long sys_mkdir_parents (const char *dir, mode_t mode) {
  if (str_eq (dir, "."))
    return 0;

  if (file_exists (dir)) {
    if (is_directory (dir))
      return 0;

    sys_errno = ENOTDIR;
    return -1;
  }

  long retval = 0;

  mode *= 2;

  char *dname = path_dirname (dir);

  if (str_eq (dname, DIR_SEP_STR))
    goto theend;

  if ((retval = sys_mkdir_parents (dname, mode)) != 0)
    goto theend;

  mode /= 2;

  mode_t m;
  if (mode > 0777) {
    mode_t mask = sys_umask (0);
    m = 0777 & ~mask;
    sys_umask (mask);
  } else
    m = mode;

  retval = syscall3 (NR_mkdirat, AT_FDCWD, (long) dir, m);

theend:
  Release (dname);
  return retval;
}
