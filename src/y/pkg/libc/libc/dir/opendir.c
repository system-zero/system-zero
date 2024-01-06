// provides: DIR *sys_opendir (const char *)
// provides: struct dirent *sys_readdir (DIR *dir)
// provides: int sys_closedir (DIR *)
// requires: stdlib/alloc.c
// requires: unistd/fcntl.c
// requires: unistd/open.c
// requires: unistd/close.c
// requires: string/mem_set.c
// requires: sys/types.h
// requires: sys/getdents.c
// requires: dir/dirent.h

struct _DIR {
  int fd;
  int buf_pos;
  int buf_end;
  char buf[2048];
};

DIR *sys_opendir (const char *path) {
  DIR *dir;

  int fd = sys_open (path, O_RDONLY|O_DIRECTORY);
  if (fd < 0)
    return NULL;

  int fl = sys_fcntl (fd, F_GETFD);
  sys_fcntl3 (fd, F_SETFD, fl|FD_CLOEXEC);

  dir = Alloc (sizeof (DIR));
  mem_set (dir, 0, sizeof (DIR));
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
    dir->buf_end += nread;
  }

  de = (void *) (dir->buf + dir->buf_pos);
  dir->buf_pos += de->d_reclen;

  //tostderr ("end %d, curpos %d reclen %d type %d name |%s|\n",
//  dir->buf_end,  dir->buf_pos, de->d_reclen, de->d_type, de->d_name);

  return de;
}

int sys_closedir (DIR *dir) {
  int ret;
  ret = sys_close (dir->fd);
  Release (dir);
  return ret;
}
