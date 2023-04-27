// provides: int sys_isatty (int)
// requires: sys/ioctl.c
// requires: sys/termios.h

int sys_isatty (int fd) {
  struct termios dummy;
  return !sys_ioctl (fd, TCGETS, &dummy);
}
