#ifndef RANDOM_HDR
#define RANDOM_HDR

typedef struct random_get_self {
  int (*entropy_bytes) (void *, size_t);
} random_get_self;

typedef struct random_self {
  random_get_self get;
  uint32_t (*new) (void);
} random_self;

typedef struct random_T {
  random_self self;
} random_T;

public random_T __init_random__ (void);
public void __deinit_random__ (random_T **);

#endif /* RANDOM_HDR */
