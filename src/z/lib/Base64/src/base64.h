#ifndef BASE64_HDR
#define BASE64_HDR

typedef struct base64_self {
  char
    *(*encode) (const char *),
    *(*decode) (const char *);
} base64_self;

typedef struct base64_T {
  base64_self self;
} base64_T;

public base64_T __init_base64__ (void);

#endif /* BASE64_HDR */
