// provides: int sys_tcsetattr (int, int, struct termios *)
// requires: unistd/termios.h
// requires: sys/ioctl.c

#define TCSANOW    0
#define TCSADRAIN  1
#define TCSAFLUSH  2

int sys_tcsetattr (int fd, int actions, struct termios *term) {
  switch (actions) {
    case TCSANOW:
      return sys_ioctl (fd, TCSETS , term);
    case TCSADRAIN:
      return sys_ioctl (fd, TCSETSW, term);
    case TCSAFLUSH:
      return sys_ioctl (fd, TCSETSF, term);
  }

  sys_errno = EINVAL;
  return -1;
}
