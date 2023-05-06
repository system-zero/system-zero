// provides: long sys_readlink (const char *, char *, size_t)
// provides: char *read_link (const char *)
// requires: unistd/fcntl.h

long sys_readlink (const char *path, char *buf, size_t len) {
#ifdef NR_readlinkat
  return syscall4 (NR_readlinkat, AT_FDCWD, (long) path, (long) buf, len);
#else
  return syscall3 (NR_readlink, (long) path, (long) buf, len);
#endif
}

char *read_link (const char *path) {
  struct stat st;

  if (sys_lstat (path, &st) == -1)
    return NULL;

  size_t size = (st.st_size ? st.st_size : MAXLEN_PATH);

  char *buf = Alloc (size);

  long r = sys_readlink (path, buf, size);

  if (r == -1) {
    Release (buf);
    return NULL;
  }

  buf[r] = '\0'; // readlink(2) doesn't add a terminating nul byte

  return buf;
}
