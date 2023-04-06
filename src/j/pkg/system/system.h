#ifndef SYSTEM_HDR
#define SYSTEM_HDR

typedef struct system_t {
  char power_state_file[MAXLEN_PATH];
} system_t;

typedef struct SystemOpts_t {
  const char *power_state_file;
} SystemOpts_t;

#define SystemOpts(...) (SystemOpts_t) { \
  .power_state_file = NULL,              \
  __VA_ARGS__ }

int sys_to_memory (system_t *);
int init_system (system_t *, SystemOpts_t);

#endif /* SYSTEM_HDR */
