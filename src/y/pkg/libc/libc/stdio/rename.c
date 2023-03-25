// provides: long sys_rename (const char *, const char *)
// requires: unistd/fcntl.h
// requires: fs/fs.h

long sys_rename (const char *oldpath, const char *newpath) {
  return syscall5 (NR_renameat2,
    AT_FDCWD, (long) oldpath, AT_FDCWD, (long) newpath, 0);
}
