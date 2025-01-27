// provides: long sys_fchownat (int, char *, uid_t, gid_t, int)
// requires: unistd/fcntl.h

long sys_fchownat (int dirfd, char *pathname, uid_t uid, gid_t gid, int flags) {
  return syscall5 (NR_fchownat, dirfd, (long) pathname, uid, gid, flags);
}
