/* If WIFEXITED(STATUS), the low-order 8 bits of the status.  */
#define WEXITSTATUS(__status__)  (((__status__) & 0xff00) >> 8)
/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define WTERMSIG(__status__)  ((__status__) & 0x7f)
/* Nonzero if STATUS indicates normal termination.  */
#define WIFEXITED(__status__)  (WTERMSIG(__status__) == 0)
/* Nonzero if STATUS indicates termination by a signal.  */
#define WIFSIGNALED(__status__) \
  (((signed char) (((__status__) & 0x7f) + 1) >> 1) > 0)

