#define LIBRARY "Random"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_RANDOM_TYPE  DONOT_DECLARE

#include <z/cenv.h>

/* Implementation from:
 * https://github.com/imneme/pcg-c-basic
 *
 * PCG Random Number Generation for C.
 *
 * Copyright 2014-2017 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 *
 * Many thanks.
 */

struct pcg_state_setseq_64 {  // Internals are *Private*.
  uint64_t state;             // RNG state.  All values are possible.
  uint64_t inc;               // Controls which RNG sequence (stream) is
                              // selected. Must *always* be odd.
};

typedef struct pcg_state_setseq_64 pcg32_random_t;

static int entropy_getbytes (void* dest, size_t size) {
  int fd = open ("/dev/urandom", O_RDONLY);
  if (fd < 0)
     return NOTOK;
  ssize_t sz = read (fd, dest, size);
  return (close (fd) is 0) and (sz is (ssize_t) size);
}

static uint32_t pcg32_random_r (pcg32_random_t *rng) {
  uint64_t oldstate = rng->state;
  rng->state = oldstate * 6364136223846793005ULL + rng->inc;
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void pcg32_srandom_r (pcg32_random_t *rng, uint64_t initstate, uint64_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  rng->state *= 6364136223846793005ULL + rng->inc;
  rng->state += initstate;
  rng->state *= 6364136223846793005ULL + rng->inc;
}

static uint32_t random_new (void) {
  uint64_t seeds[2];
  if (NOTOK is entropy_getbytes ((void*) seeds, sizeof(seeds)))
    return 0;

  pcg32_random_t rng;
  pcg32_srandom_r (&rng, seeds[0], seeds[1]);
  uint32_t r = pcg32_random_r (&rng);
  if (r >= RAND_MAX)
    r %= RAND_MAX;
  return r;
}

#define random_get_entropy_bytes entropy_getbytes
public random_T __init_random__ (void) {
  return (random_T) {
    .self = (random_self) {
      .new = random_new,
      .get = (random_get_self) {
        .entropy_bytes = random_get_entropy_bytes
      }
    }
  };
}

public void __deinit_random__ (random_T **thisp) {
  (void) thisp;
  return;
}
