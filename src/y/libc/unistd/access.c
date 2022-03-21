// provides: long sys_access (const char *, int)
// provides: #define access sys_access

#define F_OK    0
#define X_OK    1
#define W_OK    2
#define R_OK    4

long sys_access (const char *path, int mode) {
  return syscall4 (NR_faccessat, AT_FDCWD, (long) path, mode, 0);
}
