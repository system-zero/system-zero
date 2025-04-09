/* we try to implement environment, with an opinion that we believe that is not so wise
   to depend too much on this. It is usefull to set up some mostly constant strings and
   to pass those small strings to execve(), with some control which of those to pass. */

typedef struct {
  char  *env;
  /* Maximum string length ("name=value" + nulbyte) 512 bytes.
     Both name and value can not exceed 255 bytes. */
  u8  name_len;
  u8  value_len;
} env_t;

/* we don't have a mechanism to utilize them fully yet */
#define ENV_IS_CONSTANT (1 << 0)
#define ENV_IS_SHAREBLE (1 << 1)
// more can come from `man secure_getenv`

#ifndef ENV_CREATION_FLAGS
#define ENV_CREATION_FLAGS (ENV_IS_SHAREBLE)
#endif

#ifndef ENV_DEFAULT_MEM_SIZE
#define ENV_DEFAULT_MEM_SIZE 16
#endif

typedef struct {
  u16 size;
  u16 mem_size;
} EnvOps;

/* Maximum strings (1 << 16) (guarranteed to be 64536 - 1 (for last NULL) entries) */
typedef struct {
  char **environ;
  u16    size;
  u16    mem_size;

  u8    *flags;
  u8    *name_len;
} Env_t;
