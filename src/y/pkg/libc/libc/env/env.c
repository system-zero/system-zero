// provides: Env_t env_new_environ (Env_t, EnvOps)
// provides: char *get_env (const char *)
// provides: int put_env (const char *)
// provides: int set_env (const char *, const char *, int)
// provides: int unset_env (const char *)
// provides: char *env_get_string (Env_t *E, const char *, u8, int *)
// provides: int env_init (Env_t *)
// provides: int env_set (Env_t *, const char *, const char *, int)
// provides: int env_put (Env_t *, const char *)
// provides: char *env_get (Env_t *, const char *, u8)
// requires: stdlib/alloc.c
// requires: string/bytelen.c
// requires: string/str_dup.c
// requires: string/str_eq_n.c
// requires: env/env.h

char **environ = NULL;

Env_t PublicEnviron = (Env_t) {
  .environ   = NULL,
  .mem_size  = 0,
  .size      = 0,
  .flags     = NULL,
  .name_len  = NULL,
};

Env_t env_new_environ (Env_t E, EnvOps ops) {  // when null then realloc calls malloc
  E.environ   = Realloc (E.environ,   ops.mem_size * sizeof (char *) /* 4|8 bytes */);
  E.flags     = Realloc (E.flags,     ops.mem_size); // one byte * mem_size
  E.name_len  = Realloc (E.name_len,  ops.mem_size); // likewise 
  E.size      = ops.size;
  E.mem_size  = ops.mem_size;
  return E;
}

char *env_get_string (Env_t *E, const char *name, u8 name_len, int *idx) {
  if (NULL == E->environ)
    return NULL;

  char **envp = E->environ;

  char *e;

  *idx = -1;

  for (; *envp != NULL; *idx += 1, envp++) {
    if (E->name_len[*idx + 1] != name_len)
      continue;

    e = *envp;

    if (str_eq_n (e, name, name_len)) {
      if (e[name_len] == '=')
        return e;
    }
  }

  return NULL;
}

static int __env_put__ (Env_t *E, env_t env, int overwrite) {
  /* you better set it up before, but its ok */
  if (NULL == E->environ) {
    *E = env_new_environ (*E, (EnvOps) {
       .size = 0,.mem_size = ENV_DEFAULT_MEM_SIZE
     });

    for (u16 i = 0; i < E->mem_size; i++) {
      E->environ[i] = 0;
      E->flags[i] = ENV_CREATION_FLAGS;
      E->name_len[i] = 0;
    }
  }

  char **envp = E->environ;

  int idx;
  char *e = env_get_string (E, env.env, env.name_len, &idx);

  if (NULL != e) {
    if (0 == overwrite || E->flags[idx] & ENV_IS_CONSTANT)
      return 0;

    envp = E->environ + idx;

    Release (e);

    goto new;
  }

  if (E->size + 1 == E->mem_size) {
    *E = env_new_environ (*E, (EnvOps) {
       .size = E->size, .mem_size = E->mem_size * 2
     });
  }

  idx = E->size;
  envp = E->environ + E->size++;

new:
  *envp             = str_dup (env.env, env.name_len + env.value_len + 1);
  E->flags[idx]     = ENV_CREATION_FLAGS;
  E->name_len[idx]  = env.name_len;

  return 0;
}

static int __env_init__ (env_t *env, const char *e) {
  env->env = NULL;

  const char *sign = e;

  while (*sign && *sign != '=')
    sign++;

  if (*sign != '=') // either first byte is 0 or not found
    // todo return env  The caller should check name len and|or value len 
    return -1;

  env->env = (char *) e;
  env->name_len = sign - e;

  sign++;
  while (*sign++)
    env->value_len++;

  return 0;
}

char *get_env (const char *name) {
  if (NULL == name || *name == '\0')
    return NULL;

  size_t name_len = bytelen (name);
  int idx;
  char *e = env_get_string (&PublicEnviron, name, name_len, &idx);
  return (NULL == e ? NULL : e + (name_len + 1));
}

int put_env (const char *e) {
  env_t env = (env_t) {
    .env = NULL,
    .name_len = 0,
    .value_len = 0
  };

  if (-1 == __env_init__ (&env, e))
    return 0;

  if (-1 == __env_put__ (&PublicEnviron, env, 1)) // always overwrite
    return -1;

  if (environ != NULL)
    environ = PublicEnviron.environ;

  return 0;
}

static int __env_set__ (Env_t *E, const char *name, const char *value, int overwrite) {
  char e[1 << 9];

  char *p = e;
  const char *sp = name;
  u8 n = 0;
  while (*sp && n++ < ((1 << 8) - 1)) // maximum name length 255 bytes 
    *p++ = *sp++;                     // 1 byte fot the sign '=' byte

  if (*sp != '\0') {
    sys_errno = EINVAL;
    return -1;
  }

  size_t name_len = sp - name;

  *p++ = '=';

  sp = value;
  n = 0;
  while (*sp && n++ < ((1 << 8) - 1)) // maximum name length 255 bytes
    *p++ = *sp++;                     // 1 byte for the nul byte 

  size_t value_len = sp - value;

  if (*sp != '\0') {
    sys_errno = EINVAL;
    return -1;
  }

  *p = '\0';

  env_t env = (env_t) {
    .env = e,
    .name_len = name_len,
    .value_len = value_len
  };

  return __env_put__ (E, env, overwrite);
}

int set_env (const char *name, const char *value, int overwrite) {
  if (-1 == __env_set__ (&PublicEnviron, name, value, overwrite))
    return -1;

  if (environ != NULL)
    environ = PublicEnviron.environ;

  return 0;
}

int unset_env (const char *name) {
  if (name == NULL || *name == '\0') {
    sys_errno = EINVAL;
    return -1;
  }

  size_t namelen = bytelen (name);

  int idx;
  char *e = env_get_string (&PublicEnviron, name, namelen, &idx);
  if (NULL == e)
    return 0;

  Release (e);

  PublicEnviron.size--;

  char **envp      = PublicEnviron.environ   + idx;
  u8    *flags     = PublicEnviron.flags     + idx;
  u8    *name_len  = PublicEnviron.name_len  + idx;

  for (; *envp != NULL; envp++, flags++, name_len++) {
    *envp      = *(envp     + 1);
    *flags     = *(flags    + 1);
    *name_len  = *(name_len + 1);
  }

  return 0;
}

int env_put (Env_t *E, const char *e) {
  env_t env = (env_t) {
    .env = NULL,
    .name_len = 0,
    .value_len = 0
  };

  if (-1 == __env_init__ (&env, e))
    return 0;

  return __env_put__ (E, env, 1);
}

int env_set (Env_t *E, const char *name, const char *value, int overwrite) {
  return __env_set__ (E, name, value, overwrite);
}

char *env_get (Env_t *E, const char *name, u8 name_len) {
  int idx;
  char *e = env_get_string (E, name, name_len, &idx);
  if (NULL == e)
    return NULL;
  return e + (name_len + 1);
}

/* test {
// num-tests: 2
#define REQUIRE_ENV
#define REQUIRE_GETENV
#define REQUIRE_STR_EQ
#define REQUIRE_ATOI
#define REQUIRE_FORMAT

#include <libc.h>

#define ME "env interface"

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing " ME ": %s - ", total, __func__);

  const char *e = "Name";
  const char *v = "Value";

  set_env (e, v, 1);

  char *nenv = get_env (e);

  if (0 == str_eq (nenv, v)) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  e = "NAME=VALUE";

  put_env (e);

  nenv = get_env ("NAME");

  if (0 == str_eq (nenv, "VALUE")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  nenv = get_env ("Name");

  if (0 == str_eq (nenv, "Value")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  unset_env ("Name");

  nenv = get_env ("Name");

  if (NULL != nenv) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  nenv = get_env ("NAME");

  if (0 == str_eq (nenv, "VALUE")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  return retval;
}

static int second_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing " ME ": %s - ", total, __func__);

  const char *e = "Name";
  const char *v = "Value";

  Env_t E = (Env_t) { .environ = NULL };

  env_set (&E, e, v, 1);

  char *nenv = env_get (&E, e, 4);

  if (0 == str_eq (nenv, v)) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  e = "NAME=VALUE";

  env_put (&E, e);

  nenv = env_get (&E, "NAME", 4);

  if (0 == str_eq (nenv, "VALUE")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  nenv = env_get (&E, "Name", 4);

  if (0 == str_eq (nenv, "Value")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;
  total++;
  if (second_test (total) == -1) failed++;

  return failed;
}
} */
