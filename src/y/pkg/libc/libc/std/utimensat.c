// provides: long sys_utimensat (int dirfd, const char *pathname, const struct timespec times[2], int flags)
// requires: unistd/fcntl.h
// requires: time/time.h

long sys_utimensat (int dirfd, const char *pathname, const struct timespec times[2], int flags) {
  return syscall4 (NR_utimensat, dirfd, (long) pathname, (long) times, flags);
}
