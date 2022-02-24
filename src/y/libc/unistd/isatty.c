// provides: int sys_isatty (int)
// requires: sys/ioctl.c
// requires: sys/termios.h

#ifndef TCGETS
#define TCGETS   0x5401
#endif

int sys_isatty (int fd) {
  struct termios dummy;
  return !ioctl (fd, TCGETS, &dummy);
}
