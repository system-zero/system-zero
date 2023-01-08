#include <stdio.h>
#include <stdlib.h>

#define REQUIRE_FILE_EXISTS
#define REQUIRE_STRLEN
#include "../../../zlibc/zlibc.c"

#include <system.h>

int sys_to_memory_linux (system_t *this) {
  if (NULL is this->power_state_file) return -1;

  size_t comlen = this->power_state_filelen + 1 + 256;

  char com[comlen + 1];
  snprintf (com, comlen + 1,
      "/bin/sh -c \"/bin/printf \"mem\" >%s\"",
      this->power_state_file);
  return system (com);
}

int init_system (system_t *this, const char *file) {
  if (NULL is file) return -1;
  ifnot (file_exists (file)) return -1;
  this->power_state_filelen = bytelen (file);
  this->power_state_file = malloc (this->power_state_filelen + 1);
  snprintf (this->power_state_file, this->power_state_filelen + 1, "%s", file);
  return 0;
}
