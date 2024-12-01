
#ifndef ASM_OP_WRITE
#define ASM_OP_WRITE btcode_u16_write
#endif

// https://stackoverflow.com/questions/43787672/the-max-number-of-digits-in-an-int-based-on-number-of-bits/43789115#43789115
#define NUM_DIGITS_OF(_t_) (241 * sizeof (_t_) / 100 + 1)

#define ASM_OK OK
#define ASM_NOTOK NOTOK

#define ASM_IS_NULL_PTR   -2
#define ASM_AWAITING_NL   -10
#define ASM_UNEXPECTED_NUL_BYTE -20

typedef struct AsmType AsmType;

typedef i32 (*AsmOp) (AsmType *);

struct AsmType {
  BtCodeType *asmCode;
  BtCodeType *btCode;

  Map_Type *ops;

  char op[MAXLEN_OP];

  char errorMsg[MAXLEN_ERROR];
  i32 error;
};

AsmType *asm_new (u16);
void asm_release (AsmType **);
void asm_add_op (AsmType *, const char *, AsmOp);
void asm_add_code (AsmType *, BtCodeType *);
i32 asm_compile (AsmType *);
i32 asm_error (AsmType *, i32, const char *, ...);
i32 asm_get_op (AsmType *);

inline u8 asm_read_byte (AsmType *asm) {
  return *asm->asmCode->cur++;
}

inline void asm_unread_byte (AsmType *asm) {
  asm->asmCode->cur--;
}

inline u8 read_non_ws_byte (AsmType *asm) {
  u8 c = *asm->asmCode->cur++;
  while (c == ' ' || c == '\n')
    c = *asm->asmCode->cur++;
  return c;
}

