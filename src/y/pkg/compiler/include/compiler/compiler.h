typedef struct CompilerType CompilerType;

typedef struct {
  const char *asmFile;
  AsmType *asm;
  DisasmType *disasm;
  VmType *vm;
} CompilerOpts;

#define CompilerNew(...) (CompilerOpts) { \
  .asmFile = NULL,                        \
  .asm = NULL,                            \
  .disasm = NULL,                         \
  .vm = NULL,                             \
  __VA_ARGS__                             \
}

struct CompilerType {
  const char *asmFile;
  AsmType *asm;
  DisasmType  *disasm;
  VmType *vm;

  char errorMsg[VM_MAXLEN_ERROR];
  i32 error;
};

i32 compiler_error (CompilerType *, i32, const char *, ...);
CompilerType *compiler_new (CompilerOpts);
void compiler_release (CompilerType **c);
i32 compiler_compile (CompilerType *);
i32 compiler_exec (CompilerType *);
