// provides: long sys_uname (struct utsname *)
// requires: sys/uname.h

long sys_uname (struct utsname *u) {
  return syscall1 (NR_uname, (long) u);
}
