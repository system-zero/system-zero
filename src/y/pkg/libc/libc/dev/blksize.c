// as: dev_blksize
// provides: uint64_t dev_blksize (int)
// requires: sys/ioctl.c
// requires: fs/fs.h

uint64_t dev_blksize (int fd) {
  uint64_t size;
  sys_ioctl (fd, BLKGETSIZE64, &size);
  return size;
}
