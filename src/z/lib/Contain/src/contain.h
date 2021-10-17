#ifndef CONTAIN_HDR
#define CONTAIN_HDR

typedef struct contain_t contain_t;

typedef struct contain_set_self {
  int
    (*uidmap) (contain_t *, char *),
    (*gidmap) (contain_t *, char *),
    (*rootDir) (contain_t *, char *),
    (*argv) (contain_t *, char **);

} contain_set_self;

typedef struct contain_get_self {
  char *(*err_msg) (contain_t *);
} contain_get_self;

typedef struct contain_self {
  contain_set_self set;
  contain_get_self get;

  contain_t *(*new) (void);
  void (*release) (contain_t **);
  int (*run) (contain_t *);
} contain_self;

typedef struct contain_T {
  contain_self self;
} contain_T;

public contain_T __init_contain__ (void);
public void __deinit_contain__ (contain_T **);

#endif /* CONTAIN_HDR */
