// provides: int sys_fsync (int)

int sys_fsync (int fd) {
  return syscall1 (NR_fsync, fd);
} 
