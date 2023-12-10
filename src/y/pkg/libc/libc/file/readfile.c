// provides: int readfile_u (readfile_t *)
// requires: unistd/open.c
// requires: unistd/read.c
// requires: unistd/close.c
// requires: file/readfile.h

#ifndef RF_CHUNKSZ
#define RF_CHUNKSZ 256
#endif

// we don't initialize a thing
int readfile_u (readfile_t *my) {
  if (NULL == my->file) {
    sys_errno = EINVAL;
    return -1;
  }

  int fd = sys_open (my->file, O_RDONLY);
  if (fd == -1)
    return -1;

  char *buf = my->bytes;
  int n = 0;
  for (;;) {
    if (my->num_bytes + RF_CHUNKSZ + 1 > my->mem_size) {
      my->mem_size += RF_CHUNKSZ * 2;
      my->bytes = Realloc (my->bytes, my->mem_size);
      buf = my->bytes + my->num_bytes;
    }

    n = sys_read (fd, buf, RF_CHUNKSZ);
    if (n <= 0)
      break;

    buf += n;
    my->num_bytes += n;
  }

  *buf = '\0';

  sys_close (fd);

  return 0;
}
