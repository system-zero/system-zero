// provides: int sys_sigemptyset(sigset_t *)
// requires: signal/sigset.h

int sys_sigemptyset (sigset_t *set) {
  set->sig[0] = 0;
  if (_SIGSET_WORDS > 1)
    set->sig[1] = 0;

  if (_SIGSET_WORDS > 2) {
    set->sig[2] = 0;
    set->sig[3] = 0;
  }

  return 0;
}
