#ifndef SHA512_HDR
#define SHA512_HDR

/* suckless sha512 from sbase
 * public domain sha512 implementation based on fips180-3
 */

struct sha512 {
  uint64_t len;    /* processed message length */
  uint64_t h[8];   /* hash state */
  uchar buf[128];  /* message block buffer */
};

enum { SHA512_DIGEST_LENGTH = 64 };

typedef struct sha512_self {
  void
    (*init) (void *),
    (*sum) (void *, uchar *),
    (*update) (void *, const uchar *, unsigned long);
} sha512_self;

typedef struct sha512_T {
  sha512_self self;
} sha512_T;

public sha512_T __init_sha512__ (void);

#endif /* SHA512_HDR */
