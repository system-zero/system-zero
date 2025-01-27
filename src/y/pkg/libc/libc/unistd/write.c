// provides: ssize_t sys_write (int, const char *, size_t)
// provides: int fd_write (int, const char *, size_t)
// requires: time/time.h
// requires: time/nanosleep.c

ssize_t sys_write (int fd, const char *buf, size_t len) {
  return syscall3 (NR_write, fd, (long) buf, len);
}

int fd_write (int fd, const char *buf, size_t len) {
  char *s = (char *) buf;
  int numwritten = 0;

  ulong llen = len;
  size_t tbts = len;

  for (;;) {
    sys_errno = 0;

    numwritten = sys_write (fd, s, llen);

    if (-1 != numwritten) {
      if (numwritten == 0)
        break;

      tbts += numwritten;

      if (tbts == len)
        break;

      s += numwritten;
      llen -= numwritten;

      continue;
    }

    if (sys_errno == EINTR)
      continue;

    if (sys_errno != EAGAIN)
      return -1;

    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 100000000L;

    struct timespec rem;

    for (;;) {
      sys_errno = 0;

      if (0 == sys_nanosleep (&req, &rem))
        break;

      if (sys_errno != EINTR)
        return -1;

      req = rem;
      rem.tv_sec = 0;
      rem.tv_nsec = 0;
    }
  }

  return tbts;
}
