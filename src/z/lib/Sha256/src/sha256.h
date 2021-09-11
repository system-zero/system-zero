#ifndef SHA256_HDR
#define SHA256_HDR

/* suckless sha256 from sbase
 * public domain sha256 implementation based on fips180-3
 */

struct sha256 {
  uint64_t len;    /* processed message length */
  uint32_t h[8];   /* hash state */
  uchar buf[64];  /* message block buffer */
};

enum { SHA256_DIGEST_LENGTH = 32 };

typedef struct sha256_self {
  void
    (*init) (void*),
    (*sum) (void *, uchar *),
    (*update) (void *, const uchar *, unsigned long);
} sha256_self;

typedef struct sha256_T {
  sha256_self self;
} sha256_T;

public sha256_T __init_sha256__ (void);

#endif /* SHA256_HDR */
