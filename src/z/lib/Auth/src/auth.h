#ifndef LIBAUTH_H
#define LIBAUTH_H

#ifndef AUTH_TEST_PROG
#define AUTH_TEST_PROG "sudo"
#endif

#define MINUTE 60
#define HOUR   (MINUTE * 60)

#ifndef CACHED_TIME
#define CACHED_TIME (HOUR / 2)
#endif

#ifndef MIN_CACHED_TIME
#define MIN_CACHED_TIME (MINUTE)
#endif

#ifndef MAX_CACHED_TIME
#define MAX_CACHED_TIME (HOUR)
#endif

#define MAX_NUM_TRIES_AFTER_FAILURE 3

#define MAXLEN_PASSWD   1024
#define MAXLEN_USERNAME 1024

typedef struct auth_t auth_t;
typedef struct auth_prop auth_prop;

typedef int (*AuthGetPasswd_cb) (auth_t *);

typedef struct auth_get_self {
  char
    *(*user) (auth_t *),
    *(*group) (auth_t *);

  void *(*user_data) (auth_t *);

  uid_t (*uid) (auth_t *);
  gid_t (*gid) (auth_t *);
} auth_get_self;

typedef struct auth_set_self {
  int
    (*timeout) (auth_t *, time_t),
    (*num_tries) (auth_t *, int),
    (*cached_time) (auth_t *, int);

  void (*user_data) (auth_t *, void *);
} auth_set_self;

typedef struct auth_self {
  auth_set_self set;
  auth_get_self get;

  auth_t *(*new) (const char *, const char *, int);

  void
    (*release) (auth_t *),
    (*reset_hashed) (auth_t *);

  int (*check) (auth_t *);
} auth_self;

struct auth_t {
  auth_prop *prop;
};

typedef struct auth_T {
  auth_self self;
} auth_T;

auth_T __init_auth__ (void);

#endif /* LIBAUTH_H */
