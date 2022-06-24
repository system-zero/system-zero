/**
 * https://github.com/rxi/uuid4
 * Copyright (c) 2018 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 */

#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#include <z/cenv.h>

#define UUID4_LEN 37

MODULE(uuid)

static uint64_t xorshift128plus (uint64_t s[2]) {
  /* http://xorshift.di.unimi.it/xorshift128plus.c */
  uint64_t s1 = s[0];
  const uint64_t s0 = s[1];
  s[0] = s0;
  s1 ^= s1 << 23;
  s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
  return s[1] + s0;
}

static int uuid4_init (uint64_t seed[2]) {
  int res;

  FILE *fp = fopen("/dev/urandom", "r");
  if (NULL is fp)
    return NOTOK;

  #define SZ sizeof (uint64_t) * 2

  res = fread (seed, 1, SZ, fp);
  fclose (fp);

  if (res isnot SZ)
    return NOTOK;

  return OK;
}

static VALUE uuid4_generate (la_t *this) {
  (void) this;
  const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  const char *chars = "0123456789abcdef";

  uint64_t seed[2];

  if (NOTOK is uuid4_init (seed))
    return NULL_VALUE;

  union {
    unsigned char b[16];
    uint64_t word[2];
  } sa;

  sa.word[0] = xorshift128plus (seed);
  sa.word[1] = xorshift128plus (seed);

  char dst[UUID4_LEN];
  const char *p;
  int i, n;

  p = template;

  i = 0;
  int idx = 0;
  while (*p) {
    n = sa.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);

    switch (*p) {
      case 'x'  :  dst[idx] = chars[n];              i++;  break;
      case 'y'  :  dst[idx] = chars[(n & 0x3) + 8];  i++;  break;
      default   :  dst[idx] = *p;
    }

    idx++;
    p++;
  }

  dst[idx] = '\0';
  string *s = String.new_with (dst);
  return STRING(s);
}

public int __init_uuid_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "uuid_generate",         PTR(uuid4_generate), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Uuid = {
      "generate" : uuid_generate,
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_uuid_module__ (la_t *this) {
  (void) this;
  return;
}
