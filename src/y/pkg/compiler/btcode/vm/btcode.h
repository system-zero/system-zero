
#define BTCODE_END  0
#define BTCODE_BEG  1


typedef struct BtCodeType {
  uchar *beg;
  uchar *cur;
  uchar *end;
  usize mem_size;
} BtCodeType;

BtCodeType *btcode_new (usize);
void btcode_release (BtCodeType **);

i32 btcode_u8_write    (BtCodeType *, u8);
i32 btcode_u16_write   (BtCodeType *, u16);
i32 btcode_i32_write   (BtCodeType *, i32);
i32 btcode_i64_write   (BtCodeType *, i64);
i32 btcode_usize_write (BtCodeType *, usize);
// i32 btcode_f32_write   (BtCodeType *, f32);
// i32 btcode_f64_write   (BtCodeType *, f64);
i32 btcode_bytes_write (BtCodeType *, const char *, usize);

inline
u8    btcode_u8_read_u    (BtCodeType *btcode) { return *btcode->cur++; }
u16   btcode_u16_read_u   (BtCodeType *);
i32   btcode_i32_read_u   (BtCodeType *);
i64   btcode_i64_read_u   (BtCodeType *);
usize btcode_usize_read_u (BtCodeType *);
// f32   btcode_f32_read_u   (BtCodeType *);
// f64   btcode_f64_read_u   (BtCodeType *);
char *btcode_bytes_read_u (BtCodeType *, char *, usize);

inline uchar *btcode_patch_point (BtCodeType *btcode) {
  uchar *end = btcode->end;
  btcode_usize_write (btcode, 0);
  return end;
}

inline usize btcode_diff (BtCodeType *btcode, uchar *p) {
  return btcode->end - p;
}

inline i32 btcode_patch (BtCodeType *btcode, usize patch) {
  uchar *end = btcode->end;
  btcode->end = btcode->end - patch;
  btcode_usize_write (btcode, patch);
  btcode->end = end;
  return OK;
}

inline i32 btcode_ensure_bytes (BtCodeType *btcode, usize num) {
  if (btcode->cur + num - 1 > btcode->end)
    return NOTOK;
  return OK;
}

#define BtCodeEnsureMem(btcode__, sz__) do {                         \
  if (btcode__->mem_size < (btcode__->end - btcode__->beg) + sz__) { \
    btcode__->mem_size *= 2;                                         \
    btcode__ = Realloc (btcode__, btcode__->mem_size);               \
  }                                                                  \
} while (0)
