#ifndef SH_HDR
#define SH_HDR

typedef struct sh_t sh_t;

typedef struct sh_self {
  sh_t *(*new) (void);

  void
    (*release) (sh_t *),
    (*release_list) (sh_t *);

  int (*exec) (sh_t *, char *);

} sh_self;

typedef struct sh_T {
  sh_self self;
} sh_T;

public sh_T __init_sh__ (void);
public void __deinit_sh__ (sh_T **);

#endif /* SH_HDR */
