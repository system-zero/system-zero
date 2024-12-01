#include "vm_libc.h"

#include <vm/constants.h>
#include <vm/types.h>
#include <vm/btcode.h>

#include <vm/vm.h>

u16 vm_add_op (VmType *vm, VmOpOpts op) {
  if (vm->numOps == vm->numOpsAllocated) {
    vm->numOpsAllocated *= 2;
    vm->ops = Realloc (vm->ops, sizeof (VmOp) * vm->numOpsAllocated);
    vm->opNames = Realloc (vm->opNames, sizeof (char *) * vm->numOpsAllocated);
  }

  vm->ops[vm->numOps] = op.opFunc;
  vm->opNames[vm->numOps] = str_new_with (op.opName);

  return vm->numOps++;
}

void vm_add_bytecode (VmType *vm, BtCodeType *btcode) {
  vm->btcode = btcode;
  vm->btcode->cur = vm->btcode->beg;
}

i32 vm_error (VmType *vm, i32 error, const char *fmt, ...) {
  vm->error = error;

  FormatType this = NewFormat (vm->errorMsg, VM_MAXLEN_ERROR + 1, fmt);
  va_list ap;
  va_start(ap, fmt);
  vformat (&this, ap);
  va_end(ap);

  return NOTOK;
}

i32 vm_run (VmType *vm) {
  i32 retval = vm_execute_op (vm, vm_get_op (vm));
  vm->error = retval;
  if (retval == VM_EXIT)
    retval = OK;
  return retval;
}

i32 vm_btcode_end (VmType *vm) {
  (void) vm;
  return OK;
}

i32 vm_btcode_beg (VmType *vm) {
  return vm_execute_op (vm, vm_get_op (vm));
}

VmType *vm_init (VmTypeOpts opts) {
  VmType *vm = Alloc (sizeof (VmType));
  vm->btcode = NULL;
  mem_set (vm->errorMsg, 0, VM_MAXLEN_ERROR + 1);

  vm->numOpsAllocated = opts.numOps;
  vm->opNames = Alloc (sizeof (char *) * vm->numOpsAllocated);
  vm->ops = Alloc (sizeof (VmOp) * vm->numOpsAllocated);
  vm->numOps = 0;


  END_BTCODE_OP = vm_add_op (vm, VmNewOp (.opFunc = vm_btcode_end, .opName = "end.btcode"));
  BEG_BTCODE_OP = vm_add_op (vm, VmNewOp (.opFunc = vm_btcode_beg, .opName = "beg.btcode"));

  return vm;
}

void vm_deinit (VmType **vmp) {
  if (*vmp == NULL)
    return;

  for (int i = 0; i < (*vmp)->numOps; i++)
    Release ((*vmp)->ops[i]);
  Release ((*vmp)->ops);

  Release (*vmp);
  *vmp = NULL;
}
