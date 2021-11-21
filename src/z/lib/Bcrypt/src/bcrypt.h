#ifndef BCRYPT_HDR
#define BCRYPT_HDR

#define BCRYPT_HASHSIZE	64

typedef struct bcrypt_self {
  char *(*hashpw) (const char *);
  int (*verify_pw_hash) (const char *, const char *);
} bcrypt_self;

typedef struct bcrypt_T {
  bcrypt_self self;
} bcrypt_T;

public bcrypt_T __init_bcrypt__ (void);
public void __deinit_bcrypt__ (bcrypt_T **);

#endif /* BCRYPT_HDR */
