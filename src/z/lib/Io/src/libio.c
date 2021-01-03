#define LIBRARY "Io"

#define REQUIRE_UNISTD
#define REQUIRE_SELECT
#define REQUIRE_IO_TYPE DONOT_DECLARE

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

static int io_fd_read (int fd, char *buf, idx_t len) {
  if (1 > len) return NOTOK;

  char *s = buf;
  ssize_t bts;
  int tbts = 0;

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

static int io_fd_write (int fd, char *buf, idx_t len) {
  int bts;
  int tbts = len;

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

public io_T __init_io__ (void) {
  return (io_T) {
    .self = (io_self) {
      .fd = (io_fd_self) {
        .read = io_fd_read,
        .write = io_fd_write
      }
    }
  };
}
