#ifndef SMAP_HDR
#define SMAP_HDR

typedef struct Smap_t Smap_t;
typedef struct smap_t smap_t;

typedef struct smap_self {
  void
    (*release) (Smap_t *),
    (*clear) (Smap_t *);

  Smap_t *(*new) (int);

  int
    (*key_exists) (Smap_t *, const char *);

  string_t
    *(*get) (Smap_t *, const char *);

  int
    (*set) (Smap_t *, const char *, string_t *);
} smap_self;

typedef struct smap_T {
  smap_self self;
} smap_T;

public smap_T __init_smap__ (void);

#endif /* SMAP_HDR */
