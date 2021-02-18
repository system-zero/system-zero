#ifndef IMAP_HDR
#define IMAP_HDR

typedef struct Imap_t Imap_t;
typedef struct imap_t imap_t;

typedef struct imap_self {
  void
    (*release) (Imap_t *),
    (*clear) (Imap_t *);

  Imap_t *(*new) (int);

  int
    (*get) (Imap_t *, char *),
    (*key_exists) (Imap_t *, char *);

  int
    (*set) (Imap_t *, char *, int),
    (*set_with_keylen) (Imap_t *, char *);
} imap_self;

typedef struct imap_T {
  imap_self self;
} imap_T;

public imap_T __init_imap__ (void);

#endif /* IMAP_HDR */
