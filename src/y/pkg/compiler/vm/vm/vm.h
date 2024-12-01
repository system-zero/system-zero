#ifndef END_BTCODE_NAME
#define END_BTCODE_NAME "end.btcode"
#endif
#ifndef BEG_BTCODE_NAME
#define BEG_BTCODE_NAME "beg.btcode"
#endif


u16 END_BTCODE_OP;
u16 BEG_BTCODE_OP;

enum {
  VM_EXIT = -2,
  VM_TOO_MANY_OPERATIONS,
  VM_UNKNOWN_INSTRUCTION,
  VM_STACK_UNDERFLOW
};

#define VM_MAXLEN_ERROR 255
#define VM_NUM_DEFAULT_OPS 32

typedef struct VmType VmType;

typedef i32 (*VmOp) (VmType *);

typedef struct {
  VmOp  opFunc;
  const char *opName;
} VmOpOpts;

#define VmNewOp(...) (VmOpOpts) { \
  .opFunc = NULL,                 \
  .opName = NULL,                 \
  __VA_ARGS__                     \
}

typedef struct VmTypeOpts {
  u16 numOps;

  VmOp endBtcodeFunc;
  VmOp begBtcodeFunc;
  const char *endBtcodeName;
  const char *begBtcodeName;

} VmTypeOpts;

#define VmOpts(...) (VmTypeOpts) {           \
  .numOps = VM_NUM_DEFAULT_OPS,              \
  .endBtcodeFunc = vm_btcode_end,            \
  .begBtcodeFunc = vm_btcode_beg,            \
  .endBtcodeName = END_BTCODE_NAME,          \
  .begBtcodeName = BEG_BTCODE_NAME,          \
  __VA_ARGS__                                \
}

struct VmType {
  BtCodeType *btcode;

  VmOp *ops;
  char **opNames;
  u16 numOps;
  i32 numOpsAllocated;

  void *userData;

  char errorMsg[VM_MAXLEN_ERROR];
  i32 error;
};

i32 vm_error (VmType *, i32, const char *, ...);
VmType *vm_new (u16);
void vm_release (VmType **);
VmType *vm_init (VmTypeOpts);
void vm_deinit (VmType **);
void vm_add_bytecode (VmType *, BtCodeType *);
i32 vm_run (VmType *);
u16 vm_add_op (VmType *, VmOpOpts);

i32 vm_btcode_end (VmType *);
i32 vm_btcode_beg (VmType *);

inline i32 vm_execute_op (VmType *vm, u16 op) {
  if (op > vm->numOps - 1)
    return vm_error (vm, VM_UNKNOWN_INSTRUCTION, "%s: %d, unknown instruction", __func__, op);

  return vm->ops[op] (vm);
}

#define vm_get_op(vm_) btcode_u16_read_u (vm_->btcode)
