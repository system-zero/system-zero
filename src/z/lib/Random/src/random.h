#ifndef RANDOM_HDR
#define RANDOM_HDR

typedef struct random_self {
  uint32_t (*new) (void);
} random_self;

typedef struct random_T {
  random_self self;
} random_T;

public random_T __init_random__ (void);
public void __deinit_random__ (random_T **);

#endif /* RANDOM_HDR */
