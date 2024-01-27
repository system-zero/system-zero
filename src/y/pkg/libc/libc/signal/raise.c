// provides: int sys_raise (int)
// requires: signal/kill.c
// requires: unistd/getpid.c

int sys_raise (int sig) {
  return sys_kill (sys_getpid (), sig);
}
