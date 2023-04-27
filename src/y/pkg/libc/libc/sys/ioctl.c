// provides: long sys_ioctl (int, unsigned long, void *)
// requires: sys/ioctl.h

long sys_ioctl (int fd, unsigned long request, void *arg) {
  return syscall3 (NR_ioctl, fd, request, (long) arg);
}
