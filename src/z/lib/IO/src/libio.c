#define LIBRARY "IO"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_SELECT

#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_IO_TYPE      DONOT_DECLARE
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

static const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

#define CONTINUE_ON_EXPECTED_ERRNO(fd__)  \
  if (errno == EINTR) {                   \
    errno = 0;                            \
    continue;                             \
  }                                       \
  if (errno == EAGAIN) {                  \
    struct timeval tv;                    \
    fd_set read_fd;                       \
    FD_ZERO(&read_fd);                    \
    FD_SET(fd, &read_fd);                 \
    tv.tv_sec = 0;                        \
    tv.tv_usec = 100000;                  \
    select (fd__ + 1, &read_fd, NULL, NULL, &tv); \
    errno = 0;                            \
    continue;                             \
   } do {} while (0)

static idx_t io_fd_read (int fd, char *buf, idx_t len) {
  if (1 > len) return NOTOK;

  char *s = buf;
  ssize_t bts;
  idx_t tbts = 0;

  while (1) {
    if (NOTOK is (bts = read (fd, s, len))) {
      CONTINUE_ON_EXPECTED_ERRNO (fd);
      return NOTOK;
    }

    tbts += bts;
    if (tbts >= (len - 1) or bts is 0) break;

    s += bts;
  }

  if (tbts > len) return NOTOK;

  buf[tbts] = '\0';
  return tbts;
}

static idx_t io_fd_write (int fd, char *buf, idx_t len) {
  int bts;
  idx_t tbts = len;

  while (len > 0) {
    if (NOTOK is (bts = write (fd, buf, len))) {
      CONTINUE_ON_EXPECTED_ERRNO (fd);
      return NOTOK;
    }

    len -= bts;
    buf += bts;
  }

  return tbts - len;
}

static idx_t io_out_print (const char *buf) {
  idx_t nbytes = fprintf (stdout, "%s", buf);
  fflush (stdout);
  return nbytes;
}

static idx_t io_out_print_fmt (const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  idx_t nbytes = vfprintf (stdout, fmt, ap);
  va_end(ap);
  fflush (stdout);
  return nbytes;
}

static idx_t io_err_print (const char *buf) {
  idx_t nbytes = fprintf (stderr, "%s", buf);
  fflush (stdout);
  return nbytes;
}

static idx_t io_err_print_fmt (const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  idx_t nbytes = vfprintf (stderr, fmt, ap);
  va_end(ap);
  fflush (stdout);
  return nbytes;
}

static string_t *io_parse_escapes (char *buf) {
  string_t *out = String.new (256);
  char *sp = buf;

  while (*sp) {
    if (*sp isnot '\\') {
      String.append_byte (out, *sp);
      goto next;
    }

    ifnot (*(sp + 1))
      goto theerror;

    sp++;
    switch (*sp) {
      case 'a': String.append_byte (out, '\a'); break;
      case 'b': String.append_byte (out, '\b'); break;
      case 'f': String.append_byte (out, '\f'); break;
      case 't': String.append_byte (out, '\t'); break;
      case 'r': String.append_byte (out, '\r'); break;
      case 'n': String.append_byte (out, '\n'); break;
      case 'v': String.append_byte (out, '\v'); break;
      case 'e': String.append_byte (out,  033); break;
      default:  goto theerror;
    }

    next: sp++;
  }

  return out;

theerror:
  String.release (out);
  return NULL;
}

/* This is an extended version of the same function of the kilo editor at:
 * https://github.com/antirez/kilo.git
 *
 * It should work the same, under xterm, rxvt-unicode, st and linux terminals.
 * It also handles UTF8 byte sequences and it should return the integer represantation
 * of such sequence */

static utf8 io_input_getkey (int infd) {
  char c;
  int n;
  char buf[5];

  while (0 == (n = io_fd_read (infd, buf, 1)));

  if (n == -1) return -1;

  c = buf[0];

  switch (c) {
    case ESCAPE_KEY:
      if (0 == io_fd_read (infd, buf, 1))
        return ESCAPE_KEY;

      /* recent (revailed through CTRL-[other than CTRL sequence]) and unused */
      if ('z' >= buf[0] && buf[0] >= 'a')
        return 0;

      if (buf[0] == ESCAPE_KEY /* probably alt->arrow-key */)
        if (0 == io_fd_read (infd, buf, 1))
          return 0;

      if (buf[0] != '[' && buf[0] != 'O')
        return 0;

      if (0 == io_fd_read (infd, buf + 1, 1))
        return ESCAPE_KEY;

      if (buf[0] == '[') {
        if ('0' <= buf[1] && buf[1] <= '9') {
          if (0 == io_fd_read (infd, buf + 2, 1))
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
            if (io_fd_read (infd, buf, 1) == 0)
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
            if (io_fd_read (infd, buf, 1) == 0)
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
          if (io_fd_read (infd, buf, 1) == 0)
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
        if (0 >= io_fd_read (infd, &cc, 1))
          return -1;

        if (ISNOT_UTF8 ((uchar) cc)) {
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

public io_T __init_io__ (void) {
  __INIT__ (string);
  __INIT__ (ustring);

  return (io_T) {
    .self = (io_self) {
      .parse_escapes = io_parse_escapes,
      .out = (io_out_self) {
        .print = io_out_print,
        .print_fmt = io_out_print_fmt
      },
      .err = (io_err_self) {
        .print = io_err_print,
        .print_fmt = io_err_print_fmt
      },
      .input = (io_input_self) {
        .getkey = io_input_getkey
      },
      .fd = (io_fd_self) {
        .read = io_fd_read,
        .write = io_fd_write
      }
    }
  };
}
