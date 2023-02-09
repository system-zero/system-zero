#ifndef SYSTEM_HDR
#define SYSTEM_HDR

typedef struct system_t {
  char power_state_file[MAXLEN_PATH];
  size_t power_state_filelen;
} system_t;

int sys_to_memory_linux (system_t *);
int init_system (system_t *, const char *);
#endif /* SYSTEM_HDR */
