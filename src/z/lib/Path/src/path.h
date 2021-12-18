#ifndef PATH_HDR
#define PATH_HDR

typedef struct path_self {
  char
    *(*real) (const char *, char *),
    *(*basename) (char *),
    *(*basename_sans_extname) (char *),
    *(*extname) (char *),
    *(*dirname) (const char *);

  int (*is_absolute) (const char *);
  Vstring_t *(*split) (const char *);
} path_self;

typedef struct path_T {
  path_self self;
} path_T;

public path_T __init_path__ (void);

#endif /* PATH_HDR */

