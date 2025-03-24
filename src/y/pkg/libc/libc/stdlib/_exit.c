// provides: void _exit (int)
// provides: #define exit _exit

__attribute__((noreturn)) void _exit (int r) {
  long ret = syscall1 (NR_exit, r);

  ret = ( 0 > ret ? sys_errno = -ret, -1 : ret );

  ret = syscall3 (NR_write, 1, (long) ( "sys_exit failed.\n" ), 19);
  for(;;);
}
