// provides: DIR *sys_opendir (const char *)
// provides: struct dirent *sys_readdir (DIR *dir)
// provides: int sys_closedir (DIR *)
// requires: stdlib/alloc.c
// requires: unistd/fcntl.c
// requires: unistd/open.c
// requires: unistd/close.c
// requires: sys/types.h
// requires: sys/getdents.c
// requires: dir/dirent.h

DIR *sys_opendir (const char *path) {
  DIR *dir;

  int fd = sys_open (path, O_RDONLY|O_DIRECTORY);
  if (fd < 0)
    return NULL;

  int fl = sys_fcntl (fd, F_GETFD);
  sys_fcntl3 (fd, F_SETFD, fl|FD_CLOEXEC);

  dir = Alloc (sizeof (DIR));

  unsigned char *ptr = (void *) dir;

  for (size_t i = 0; i < sizeof (DIR); i++)
    *ptr++ = 0;

  dir->fd = fd;

  return dir;
}

struct dirent *sys_readdir (DIR *dir) {
  struct dirent *de;

  if (dir->buf_pos >= dir->buf_end) {
    int nread = sys_getdents (dir->fd, (void *) dir->buf, sizeof (dir->buf));
    if (nread <= 0)
      return NULL;

    dir->buf_pos = 0;
    dir->buf_end = nread;
  }

  de = (void *) (dir->buf + dir->buf_pos);
  dir->buf_pos += de->d_reclen;

  return de;
}

int sys_closedir (DIR *dir) {
  int ret;
  ret = sys_close (dir->fd);
  Release (dir);
  return ret;
}
