#define LIBRARY "IO"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_SELECT

#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_IO_TYPE      DONOT_DECLARE

#include <z/cenv.h>

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

  if (tbts >= len) return NOTOK;

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

static idx_t io_print (const char *buf) {
  idx_t nbytes = fprintf (stdout, "%s", buf);
  fflush (stdout);
  return nbytes;
}

static idx_t io_print_fmt (const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  idx_t nbytes = vfprintf (stdout, fmt, ap);
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

public io_T __init_io__ (void) {
  __INIT__ (string);

  return (io_T) {
    .self = (io_self) {
      .print = io_print,
      .print_fmt = io_print_fmt,
      .parse_escapes = io_parse_escapes,
      .fd = (io_fd_self) {
        .read = io_fd_read,
        .write = io_fd_write
      }
    }
  };
}
