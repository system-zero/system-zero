#include "btcode_libc.h"

#include <vm/constants.h>
#include <vm/types.h>
#include <vm/btcode.h>

i32 btcode_u8_write (BtCodeType *btcode, u8 v) {
  BtCodeEnsureMem (btcode, sizeof (v));
  *btcode->end++ = v;
  return OK;
}

i32 btcode_u16_write (BtCodeType *btcode, u16 v) {
  BtCodeEnsureMem (btcode, sizeof (v));
  *btcode->end++ =  v & 0xff;
  *btcode->end++ = (v >> 8) & 0xff;
  return OK;
}

i32 btcode_i32_write (BtCodeType *btcode, i32 v) {
  BtCodeEnsureMem (btcode, sizeof (v));
  *btcode->end++ =  v & 0xff;
  *btcode->end++ = (v >> 8)  & 0xff;
  *btcode->end++ = (v >> 16) & 0xff;
  *btcode->end++ = (v >> 24) & 0xff;
  return OK;
}

i32 btcode_i64_write (BtCodeType *btcode, i64 v) {
  BtCodeEnsureMem (btcode, sizeof (v));
  *btcode->end++ =  v & 0xff;
  *btcode->end++ = (v >> 8)  & 0xff;
  *btcode->end++ = (v >> 16) & 0xff;
  *btcode->end++ = (v >> 24) & 0xff;
  *btcode->end++ = (v >> 32) & 0xff;
  *btcode->end++ = (v >> 40) & 0xff;
  *btcode->end++ = (v >> 48) & 0xff;
  *btcode->end++ = (v >> 56) & 0xff;
  return OK;
}

i32 btcode_usize_write (BtCodeType *btcode, usize v) {
#if __WORDSIZE == 8
  return btcode_i64_write (btcode, v);
#else
  return btcode_i32_write (btcode, v);
#endif
}

#if 0
i32 btcode_f32_write (BtCodeType *btcode, f32 v) {
  union {
    i32 i;
    f32 f;
  } u;

  u.f = v;
  return btcode_i32_write (btcode, u.i);
}

i32 btcode_f64_write (BtCodeType *btcode, f64 v) {
  union {
    i64 i;
    f64 d;
  } u;

  u.d = v;
  return btcode_i64_write (btcode, u.i);
}
#endif

i32 btcode_bytes_write (BtCodeType *btcode, const char *s, usize sz) {
  btcode_usize_write (btcode, sz);

  BtCodeEnsureMem (btcode, sz);

  for (usize i = 0; i < sz; i++)
    *btcode->end++ = s[i];

  return OK;
}

u16 btcode_u16_read_u (BtCodeType *btcode) {
  u8 v1 = btcode_u8_read_u (btcode);
  u8 v2 = btcode_u8_read_u (btcode);
  return (u16) v1 | ((u16) v2 << 8);
}

i32 btcode_i32_read_u (BtCodeType *btcode) {
  u8 v1 = btcode_u8_read_u (btcode);
  u8 v2 = btcode_u8_read_u (btcode);
  u8 v3 = btcode_u8_read_u (btcode);
  u8 v4 = btcode_u8_read_u (btcode);
  return (i32) v1 | ((i32) v2 << 8) | ((i32) v3 << 16) | ((i32) v4 << 24);
}

i64 btcode_i64_read_u (BtCodeType *btcode) {
  u8 v1 = btcode_u8_read_u (btcode);
  u8 v2 = btcode_u8_read_u (btcode);
  u8 v3 = btcode_u8_read_u (btcode);
  u8 v4 = btcode_u8_read_u (btcode);
  u8 v5 = btcode_u8_read_u (btcode);
  u8 v6 = btcode_u8_read_u (btcode);
  u8 v7 = btcode_u8_read_u (btcode);
  u8 v8 = btcode_u8_read_u (btcode);
  return (i64) v1 |
    ((i64) v2 << 8)  |
    ((i64) v3 << 16) |
    ((i64) v4 << 24) |
    ((i64) v5 << 32) |
    ((i64) v6 << 40) |
    ((i64) v7 << 48) |
    ((i64) v8 << 56);
}

usize btcode_usize_read_u (BtCodeType *btcode) {
#if __WORDSIZE == 64
  return (usize) btcode_i64_read_u (btcode);
#else
  return (usize) btcode_i32_read_u (btcode);
#endif
}

#if 0
f32 btcode_f32_read_u (BtCodeType *btcode) {
  union {
    i32 i;
    f32 f;
  } u;

  u.i = btcode_i32_read_u (btcode);
  return u.f;
}

f64 btcode_f64_read_u (BtCodeType *btcode) {
  union {
    i32 i;
    f32 d;
  } u;

  u.i = btcode_i64_read_u (btcode);
  return u.d;
}
#endif

char *btcode_bytes_read_u (BtCodeType *btcode, char *s, usize sz) {
  for (usize i = 0; i < sz; i++)
    s[i] = btcode_u8_read_u (btcode);
  return s;
}

BtCodeType *btcode_new (usize size) {
  BtCodeType *btcode = Alloc (sizeof (BtCodeType));
  btcode->beg = Alloc (size);
  btcode->end = btcode->cur = btcode->beg;
  btcode->mem_size = size;
  return btcode;
}

void btcode_release (BtCodeType **btcodep) {
  if (*btcodep == NULL) return;

  Release ((*btcodep)->beg);
  Release (*btcodep);

  *btcodep = NULL;
}
