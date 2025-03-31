// provides: __attribute__((noreturn)) void _start (void)
// provides: __attribute__((noreturn)) void __main__ (int, char **)
// requires: stdlib/_exit.c
// requires: std/environ.h

extern int main (int, char **);

char **environ = NULL;

__attribute__((noreturn)) void __main__ (int argc, char **argv) {
  environ = argv + (argc + 1);
  int retval = main (argc, argv);
  _exit (retval);
}

#if defined(__i386__) || defined(__i386)
__attribute__ ((noreturn)) void _start (void) {
   __asm__ volatile (
     "popl  %ecx\n\t"
     "movl  %esp, %eax\n\t"
     "pushl %ecx\n\t"
     "pushl %eax\n\t"
     "pushl %ecx\n\t"
     "call  __main__"
  );

  for(;;);
}

#elif defined(__x86_64__) || defined(__amd64__) || defined(__amd64)

__attribute__((noreturn)) void _start (void) {
   __asm__ volatile (
     "movq 0(%rsp), %rdi\n\t"
     "leaq 8(%rsp), %rsi\n\t"
     "call  __main__"
  );

  for (;;);
}

#endif
