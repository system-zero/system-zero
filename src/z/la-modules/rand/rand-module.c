#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

/* https://github.com/imneme/pcg-c-basic
 * PCG Random Number Generation for C.
 *
 * Copyright 2014-2017 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 */

struct pcg_state_setseq_64 {  // Internals are *Private*.
  uint64_t state;             // RNG state.  All values are possible.
  uint64_t inc;               // Controls which RNG sequence (stream) is
                              // selected. Must *always* be odd.
};

typedef struct pcg_state_setseq_64 pcg32_random_t;

static int entropy_getbytes(void* dest, size_t size) {
  int fd = open ("/dev/urandom", O_RDONLY);
  if (fd < 0)
     return LA_NOTOK;
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

static VALUE rand_new (la_t *this) {
  (void) this;
  uint64_t seeds[2];
  if (LA_NOTOK is entropy_getbytes ((void*) seeds, sizeof(seeds)))
    return NOTOK_VALUE;

  pcg32_random_t rng;
  pcg32_srandom_r (&rng, seeds[0], seeds[1]);
  uint32_t r = pcg32_random_r (&rng);
  if (r >= RAND_MAX)
    r %= RAND_MAX;
  return INT(r);
}

#define EvalString(...) #__VA_ARGS__

public int __init_rand_module__ (la_t *this) {
  __INIT_MODULE__(this);

  (void) vmapType;
  (void) stringType;

  LaDefCFun lafuns[] = {
    { "rand_new",  PTR(rand_new), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Rand = {
      "new" : rand_new
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_rand_module__ (la_t *this) {
  (void) this;
  return;
}
