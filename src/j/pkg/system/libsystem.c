
#define REQUIRE_FILE_EXISTS
#define REQUIRE_BYTELEN
#define REQUIRE_STR_COPY
#define REQUIRE_FD

#include <libc.h>

#include <system.h>

int sys_to_memory (system_t *this) {
  // if (NULL == this->power_state_file) return -1;

  int fd = sys_open (this->power_state_file, O_RDWR);
  if (fd == -1) return -1;

  const char *mem = "mem\n";
  sys_write (fd, mem, sizeof (mem));
  sys_close (fd);
  return 0;
}

int init_system (system_t *this, SystemOpts_t opts) {
  if (NULL == opts.power_state_file) return -1;
  const char *file = opts.power_state_file;
  ifnot (file_exists (file)) return -1;
  size_t len = bytelen (file);
  if (len + 1 > MAXLEN_PATH) return -1;
  str_copy (this->power_state_file, MAXLEN_PATH, file, len);

  return 0;
}
