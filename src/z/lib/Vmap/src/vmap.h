#ifndef VMAP_HDR
#define VMAP_HDR

typedef struct Vmap_t Vmap_t;
typedef struct vmap_t vmap_t;

typedef void (*VmapRelease_cb) (void *);
typedef void *(*VmapCopy_cb) (void *, void *);

typedef struct vmap_self {
  void
    (*release) (Vmap_t *),
    (*clear) (Vmap_t *);

  Vmap_t
    *(*new) (int),
    *(*clone) (Vmap_t *, VmapCopy_cb, void *);

  int
    (*num_keys) (Vmap_t *),
    (*key_exists) (Vmap_t *, char *);

  void
    *(*pop) (Vmap_t *, char *),
    *(*get) (Vmap_t *, char *);

  int
    (*set) (Vmap_t *, char *, void *, VmapRelease_cb, int);

  string **(*keys) (Vmap_t *);

  size_t (*size_of) (void);
} vmap_self;

typedef struct vmap_T {
  vmap_self self;
} vmap_T;

public vmap_T __init_vmap__ (void);

#endif /* VMAP_HDR */
