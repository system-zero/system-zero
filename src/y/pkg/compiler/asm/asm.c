#include "asm_libc.h"
#include <vm/constants.h>
#include <vm/types.h>
#include <vm/btcode.h>
#include <vm/asm.h>


void asm_add_op (AsmType *asm, const char *opname, AsmOp op) {
  map_set (asm->ops, (char *) opname, op, NULL);
}

void asm_add_code (AsmType *asm, BtCodeType *code) {
  asm->asmCode = code;
  asm->asmCode->cur = asm->asmCode->beg;
}

i32 asm_get_op (AsmType *asm) {
  if (*(asm->asmCode->cur++) != '\n') { // strict
    asm_error (asm, ASM_AWAITING_NL, "%s(): awaiting a new line", __func__);
    tostderr ("%s\n", asm->error);
    return ASM_NOTOK;
  }

  u8 c = read_non_ws_byte (asm);

  if (c == '\0') {
    ASM_OP_WRITE (asm->btCode, BTCODE_END);
    return ASM_OK;
  }

  int idx = 0;

  while (1) {
    if (c == '\0') {
      asm_error (asm, ASM_UNEXPECTED_NUL_BYTE, "%s(): unexpected nul byte", __func__);
      tostderr ("%s\n", asm->error);
      return ASM_NOTOK;
    }

    if (c == ' ' || c == '\n') {
      asm->op[idx] = '\0';
      break;
    }

    asm->op[idx++] = c;

    c = asm_read_byte (asm);
  }

  AsmOp op = map_get_value (asm->ops, asm->op);
  //tostderr ("fun |%s|\n", asm->op);
  if (NULL == op) {
    // ASM_ERROR
    tostderr ("op is null\n");
    return ASM_NOTOK;
  }

  return op (asm);
}

i32 asm_compile (AsmType *asm) {
  if (NULL == asm)
    return ASM_IS_NULL_PTR;
  return OK;
}

i32 asm_error (AsmType *asm, i32 error, const char *fmt, ...) {
  asm->error = error;

  FormatType this = NewFormat (asm->errorMsg, MAXLEN_ERROR + 1, fmt);
  va_list ap;
  va_start(ap, fmt);
  vformat (&this, ap);
  va_end(ap);

  return NOTOK;
}

AsmType *asm_new (u16 num_ops) {
  AsmType *asm = Alloc (sizeof (AsmType));

  asm->asmCode = NULL;

  asm->btCode = btcode_new (1024);

  asm->error = OK;
  mem_set (asm->errorMsg, 0, MAXLEN_ERROR);

  asm->ops = map_new (num_ops);
  return asm;
}

void asm_release (AsmType **asmp) {
  if (*asmp == NULL) return;

  Release ((*asmp)->btCode);

  map_release (&(*asmp)->ops);

  Release (*asmp);
  *asmp = NULL;
}
