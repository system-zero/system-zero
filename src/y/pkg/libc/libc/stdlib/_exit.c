// provides: void _exit (int)

// from rt0

void _exit (int r) {
  long ret = syscall1 (NR_exit, r);

  ret = ( 0 > ret ? sys_errno = -ret, -1 : ret );

  ret = syscall3 (NR_write,
                  1,
                  ( long )( "SYS_exit failed.\n" ),
                  19 );
  for(;;);
}
