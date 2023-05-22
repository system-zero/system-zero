#define REQUIRE_DEV_BLKSIZE
#define REQUIRE_OPEN
#define REQUIRE_CLOSE

#include <libc.h>
#include <dev.h>

uint64_t device_blksize (const char *dev) {
  int fd = sys_open (dev, O_RDONLY);
  if (-1 is fd)
    return 0;

  uint64_t size = dev_blksize (fd);
  sys_close (fd);

  return size;
}
