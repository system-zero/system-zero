#ifndef SMAP_HDR
#define SMAP_HDR

typedef struct smap_t smap_t;

struct smap_t {
  char *key;
  string_t *val;
  smap_t *next;
};

typedef struct Smap_t {
  smap_t **slots;
  size_t
    num_slots,
    num_keys;
} Smap_t;

typedef struct smap_self {
  void
    (*release) (Smap_t *),
    (*clear) (Smap_t *);

  Smap_t *(*new) (int);

  int
    (*key_exists) (Smap_t *, char *);

  string_t
    *(*get) (Smap_t *, char *);

  uint
    (*set) (Smap_t *, char *, string_t *);
} smap_self;

typedef struct smap_T {
  smap_self self;
} smap_T;

public smap_T __init_smap__ (void);

#endif /* SMAP_HDR */
