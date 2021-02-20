#ifndef ERROR_HDR
#define ERROR_HDR

#define EUNKNOWN 134

typedef struct error_self {
  char
    *(*errno_name) (int),
    *(*errno_name_s) (int, char *, size_t),
    *(*errno_string) (int),
    *(*errno_string_s) (int, char *, size_t);

  int (*exists) (int);

} error_self;

typedef struct error_T {
  error_self self;
} error_T;

public error_T __init_error__ (void);

#endif /* ERROR_HDR */
