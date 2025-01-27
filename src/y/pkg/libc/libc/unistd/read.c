// provides: long sys_read (int, char *, size_t)
// provides: int fd_read (int, char *, size_t)
// requires: time/nanosleep.c

long sys_read (int fd, char *buf, size_t len) {
  return syscall3 (NR_read, fd, (long) buf, len);
}

int fd_read (int fd, char *buf, size_t len) {
  char *s = buf;
  size_t tbts = 0;
  ssize_t numread;
  ulong llen = len;

  for (;;) {
    readbuf:

    sys_errno = 0;

    numread = sys_read (fd, s, llen);

    if (-1 != numread) {
      if (numread == 0)
        break;

      tbts += numread;

      if (tbts >= len - 1)
        break;

      s += numread;
      llen -= numread;

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

      long r = sys_nanosleep (&req, &rem);
      if (0 == r)
        goto readbuf;

      if (sys_errno != EINTR)
        return -1;

      req = rem;
      rem.tv_sec = 0;
      rem.tv_nsec = 0;
    }
  }

  if (tbts > len) return -1;

  return tbts;
}
