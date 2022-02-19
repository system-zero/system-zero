// provides: int isatty (int)
// depends: sys/ioctl.c
// depends: sys/termios.h

#ifndef TCGETS
#define TCGETS   0x5401
#endif

int isatty (int fd) {
  struct termios dummy;
  return !ioctl (fd, TCGETS, &dummy);
}
