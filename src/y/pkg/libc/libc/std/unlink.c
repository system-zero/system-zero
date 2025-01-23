// provides: long sys_unlink (const char *)

long sys_unlink (const char *file) {
 return syscall3 (NR_unlinkat, AT_FDCWD, (long) file, 0);
}
