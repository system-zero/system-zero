// provides: int sys_tcgetattr (int, struct termios *)
// requires: unistd/termios.h
// requires: sys/ioctl.c

int sys_tcgetattr (int fd, struct termios *term) {
  return sys_ioctl (fd, TCGETS, term);
}
