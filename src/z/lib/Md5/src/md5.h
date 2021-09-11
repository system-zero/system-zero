#ifndef MD5_HDR
#define MD5_HDR

/* suckless md5 from sbase
 * public domain md5 implementation based on rfc1321 and libtomcrypt
 */
struct md5 {
  uint64_t len;    /* processed message length */
  uint32_t h[4];   /* hash state */
  uchar buf[64];   /* message block buffer */
};

enum { MD5_DIGEST_LENGTH = 16 };

typedef struct md5_self {
  void
    (*init) (void *),
    (*sum) (void *, uchar *),
    (*update) (void *, const uchar *, unsigned long);
} md5_self;

typedef struct md5_T {
  md5_self self;
} md5_T;

public md5_T __init_md5__ (void);

#endif /* MD5_HDR */
