#ifndef OS_HDR
#define OS_HDR

typedef struct os_get_self {
  char
    *(*pwdir) (uid_t),
    *(*pwname) (uid_t),
    *(*grname) (gid_t);

  uid_t (*pwuid) (char *);
  gid_t (*grgid) (char *);
} os_get_self;

typedef struct os_mode_self {
  char  *(*stat_to_string) (char *, mode_t);
} os_mode_self;

typedef struct os_self {
  os_get_self get;
  os_mode_self mode;
} os_self;

typedef struct os_T {
  os_self self;
} os_T;

public os_T __init_os__ (void);
public void __deinit_os__ (os_T **);

#endif /* OS_HDR */
