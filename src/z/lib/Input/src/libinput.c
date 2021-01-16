#define LIBRARY "Input"

#define REQUIRE_UNISTD
#define REQUIRE_TERMIOS
#define REQUIRE_SYS_SELECT
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_INPUT_TYPE   DONOT_DECLARE
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

struct termios orig_mode, raw_mode;

#ifndef isnotatty
#define isnotatty(fd__) (0 == isatty ((fd__)))
#endif

static int input_raw_mode (int infd) {
   if (isnotatty (infd))
     return -1;

  while (-1 == tcgetattr (infd, &orig_mode))
    if (errno != EINTR)
      return -1;

  raw_mode = orig_mode;
  raw_mode.c_iflag &= ~(INLCR|ICRNL|IXON|ISTRIP);
  raw_mode.c_cflag |= (CS8);
  raw_mode.c_oflag &= ~(OPOST);
  raw_mode.c_lflag &= ~(ECHO|ISIG|ICANON|IEXTEN);
  raw_mode.c_lflag &= NOFLSH;
  raw_mode.c_cc[VEOF] = 1;
  raw_mode.c_cc[VMIN] = 0;   /* 1 */
  raw_mode.c_cc[VTIME] = 1;  /* 0 */

  while (-1 == tcsetattr (infd, TCSAFLUSH, &raw_mode))
    if (errno != EINTR)
      return -1;

  return 0;
}

static int input_orig_mode (int infd) {
  if (isnotatty (infd))
    return -1;

  while (-1 != tcsetattr (infd, TCSAFLUSH, &orig_mode))
    if (errno != EINTR)
      return -1;

  return 0;
}

/* The only reference found for the next code, and from the last research,
 * was at the julia programming language sources (this code and the functions
 * that make use of it, is atleast 6/7 years old, during a non network season).
 */
static const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

#ifndef CONTINUE_ON_EXPECTED_ERRNO
#define CONTINUE_ON_EXPECTED_ERRNO(fd__)          \
  if (errno == EINTR) continue;                   \
  if (errno == EAGAIN) {                          \
    struct timeval tv;                            \
    fd_set read_fd;                               \
    FD_ZERO(&read_fd);                            \
    FD_SET(fd, &read_fd);                         \
    tv.tv_sec = 0;                                \
    tv.tv_usec = 100000;                          \
    select (fd__ + 1, &read_fd, NULL, NULL, &tv); \
    continue;                                     \
   } do {} while (0)
#endif

#ifndef IS_UTF8
#define IS_UTF8(c)   (((c) & 0xC0) == 0x80)
#endif

#ifndef isnotutf8
#define isnotutf8(c)  IS_UTF8 (c) == 0
#endif

static int fd_read (int fd, char *buf, size_t len) {
  if (1 > len)
    return -1;

  char *s = buf;
  ssize_t bts;
  int tbts = 0;

  while (1) {
    if (-1 == (bts = read (fd, s, len))) {
      CONTINUE_ON_EXPECTED_ERRNO (fd);
      return -1;
    }

    tbts += bts;
    if (tbts == (int) len || bts == 0)
      break;

    s += bts;
  }

  buf[tbts] = '\0';
  return bts;
}

/* This is an extended version of the same function of the kilo editor at:
 * https://github.com/antirez/kilo.git
 *
 * It should work the same, under xterm, rxvt-unicode, st and linux terminals.
 * It also handles UTF8 byte sequences and it should return the integer represantation
 * of such sequence */

static utf8 input_getkey (int infd) {
  char c;
  int n;
  char buf[5];

  while (0 == (n = fd_read (infd, buf, 1)));

  if (n == -1) return -1;

  c = buf[0];

  switch (c) {
    case ESCAPE_KEY:
      if (0 == fd_read (infd, buf, 1))
        return ESCAPE_KEY;

      /* recent (revailed through CTRL-[other than CTRL sequence]) and unused */
      if ('z' >= buf[0] && buf[0] >= 'a')
        return 0;

      if (buf[0] == ESCAPE_KEY /* probably alt->arrow-key */)
        if (0 == fd_read (infd, buf, 1))
          return 0;

      if (buf[0] != '[' && buf[0] != 'O')
        return 0;

      if (0 == fd_read (infd, buf + 1, 1))
        return ESCAPE_KEY;

      if (buf[0] == '[') {
        if ('0' <= buf[1] && buf[1] <= '9') {
          if (0 == fd_read (infd, buf + 2, 1))
            return ESCAPE_KEY;

          if (buf[2] == '~') {
            switch (buf[1]) {
              case '1': return HOME_KEY;
              case '2': return INSERT_KEY;
              case '3': return DELETE_KEY;
              case '4': return END_KEY;
              case '5': return PAGE_UP_KEY;
              case '6': return PAGE_DOWN_KEY;
              case '7': return HOME_KEY;
              case '8': return END_KEY;
              default: return 0;
            }
          } else if (buf[1] == '1') {
            if (fd_read (infd, buf, 1) == 0)
              return ESCAPE_KEY;

            switch (buf[2]) {
              case '1': return FN_KEY(1);
              case '2': return FN_KEY(2);
              case '3': return FN_KEY(3);
              case '4': return FN_KEY(4);
              case '5': return FN_KEY(5);
              case '7': return FN_KEY(6);
              case '8': return FN_KEY(7);
              case '9': return FN_KEY(8);
              default: return 0;
            }
          } else if (buf[1] == '2') {
            if (fd_read (infd, buf, 1) == 0)
              return ESCAPE_KEY;

            switch (buf[2]) {
              case '0': return FN_KEY(9);
              case '1': return FN_KEY(10);
              case '3': return FN_KEY(11);
              case '4': return FN_KEY(12);
              default: return 0;
            }
          } else { /* CTRL_[key other than CTRL sequence] */
                   /* lower case */
            if (buf[2] == 'h')
              return INSERT_KEY; /* sample/test (logically return 0) */
            else
              return 0;
          }
        } else if (buf[1] == '[') {
          if (fd_read (infd, buf, 1) == 0)
            return ESCAPE_KEY;

          switch (buf[0]) {
            case 'A': return FN_KEY(1);
            case 'B': return FN_KEY(2);
            case 'C': return FN_KEY(3);
            case 'D': return FN_KEY(4);
            case 'E': return FN_KEY(5);

            default: return 0;
          }
        } else {
          switch (buf[1]) {
            case 'A': return ARROW_UP_KEY;
            case 'B': return ARROW_DOWN_KEY;
            case 'C': return ARROW_RIGHT_KEY;
            case 'D': return ARROW_LEFT_KEY;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
            case 'P': return DELETE_KEY;

            default: return 0;
          }
        }
      } else if (buf[0] == 'O') {
        switch (buf[1]) {
          case 'A': return ARROW_UP_KEY;
          case 'B': return ARROW_DOWN_KEY;
          case 'C': return ARROW_RIGHT_KEY;
          case 'D': return ARROW_LEFT_KEY;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
          case 'P': return FN_KEY(1);
          case 'Q': return FN_KEY(2);
          case 'R': return FN_KEY(3);
          case 'S': return FN_KEY(4);

          default: return 0;
        }
      }
    break;

  default:
    if (c < 0) {
      int len = Ustring.charlen ((uchar) c);
      utf8 code = 0;
      code += (uchar) c;

      int idx;
      int invalid = 0;
      char cc;

      for (idx = 0; idx < len - 1; idx++) {
        if (0 >= fd_read (infd, &cc, 1))
          return -1;

        if (isnotutf8 ((uchar) cc)) {
          invalid = 1;
        } else {
          code <<= 6;
          code += (uchar) cc;
        }
      }

      if (invalid)
        return -1;

      code -= offsetsFromUTF8[len-1];
      return code;
    }

    if (127 == c) return BACKSPACE_KEY;

    return c;
  }

  return -1;
}

public input_T __init_input__ (void) {
  __INIT__ (ustring);

  return (input_T) {
    .self = (input_self) {
      .getkey = input_getkey,
      .raw_mode = input_raw_mode,
      .orig_mode = input_orig_mode
    }
  };
}
