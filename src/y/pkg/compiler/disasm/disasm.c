#include "disasm_libc.h"

#include <vm/constants.h>
#include <vm/types.h>
#include <vm/btcode.h>
#include <vm/disasm.h>

i32 disasm_error (DisasmType *dis, i32 error, const char *fmt, ...) {
  dis->error = error;

  FormatType this = NewFormat (dis->errorMsg, MAXLEN_ERROR + 1, fmt);
  va_list ap;
  va_start(ap, fmt);
  vformat (&this, ap);
  va_end(ap);

  return NOTOK;
}

i32 disasm_add_op (DisasmType *dis, DisasmOp op) {
  if (dis->numOps == dis->numOpsAllocated) {
    dis->numOpsAllocated *= 2;
    dis->ops = Realloc (dis->ops, sizeof (DisasmOp) * dis->numOpsAllocated);
  }

  dis->ops[dis->numOps++] = op;
  return OK;
}

DisasmType *disasm_new (BtCodeType *btcode, u16 num_ops) {
  DisasmType *dis = Alloc (sizeof (DisasmType));
  dis->numOpsAllocated = num_ops;
  dis->ops = Alloc (sizeof (DisasmOp) * dis->numOpsAllocated);
  dis->numOps = 0;
  dis->btCode = btcode;
  dis->output = string_new (256);
  dis->error = OK;
  mem_set (dis->errorMsg, 0, MAXLEN_ERROR);
  return dis;
}

void disasm_release (DisasmType **disp) {
  if (NULL == *disp) return;

  string_release ((*disp)->output);
  Release ((*disp)->ops);
  Release (*disp);
  *disp = NULL;
}

