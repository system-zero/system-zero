#include "compiler_libc.h"

#include <vm/constants.h>
#include <vm/types.h>
#include <vm/btcode.h>
#include <vm/asm.h>
#include <vm/disasm.h>
#include <vm/vm.h>
#include <compiler/compiler.h>

i32 compiler_error (CompilerType *comp, i32 error, const char *fmt, ...) {
  comp->error = error;

  FormatType this = NewFormat (comp->errorMsg, MAXLEN_ERROR + 1, fmt);
  va_list ap;
  va_start(ap, fmt);
  vformat (&this, ap);
  va_end(ap);

  return NOTOK;
}

CompilerType *compiler_new (CompilerOpts opts) {
  CompilerType *c = Alloc (sizeof (CompilerType));

  c->asm = opts.asm;
  c->disasm = opts.disasm;
  c->vm = opts.vm;

  c->error = OK;
  mem_set (c->errorMsg, 0, MAXLEN_ERROR);

  c->asmFile = opts.asmFile;

  return c;
}

i32 compiler_exec (CompilerType *c) {
  VmType *vm = c->vm;
  return vm_run (vm);
}

i32 compiler_compile (CompilerType *c) {
  AsmType *asm = c->asm;
  return asm_compile (asm);
}

void compiler_release (CompilerType **cp) {
  if (NULL == *cp)
    return;

  Release (cp);
  *cp = NULL;
}
