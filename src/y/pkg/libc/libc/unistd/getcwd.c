// provides: long sys_getcwd (char *, size_t)
// provides: char *current_directory (void)
// requires: stdlib/alloc.c

long sys_getcwd (char* buf, size_t size) {
  return syscall2 (NR_getcwd, (long) buf, size);
}

char *current_directory (void) {
  size_t size = 128;
  char *dir = Alloc (size);

  while ((-1 == sys_getcwd (dir, size))) {
    if (sys_errno != ERANGE) {
      Release (dir);
      return NULL;
    }

    size += (size / 2);
    dir = Realloc (dir, size);
  }

  return dir;
}
