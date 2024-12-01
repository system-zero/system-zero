enum {
  DISASM_UNKNOWN_INSTRUCTION = 1
};

typedef struct DisasmType DisasmType;

typedef i32 (*DisasmOp) (DisasmType *);

struct DisasmType {
  DisasmOp *ops;
  u16 numOps;
  i32 numOpsAllocated;

  BtCodeType *btCode;

  i32  error;
  char errorMsg[MAXLEN_ERROR];

  string *output;
};

DisasmType *disasm_new (BtCodeType *, u16);
void disasm_release (DisasmType **);
i32 disasm_add_op (DisasmType *, DisasmOp);
i32 disasm_error (DisasmType *, i32, const char *, ...);

inline i32 disasm_op (DisasmType *dis, u16 op) {
  if (op > dis->numOps - 1)
    return disasm_error (dis, DISASM_UNKNOWN_INSTRUCTION, "%s: %d, unknown instruction", __func__, op);

  return dis->ops[op] (dis);
}
