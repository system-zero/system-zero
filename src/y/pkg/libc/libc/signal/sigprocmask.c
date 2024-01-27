// provides: long sys_sigprocmask (int, const sigset_t *, sigset_t *)
// requires: signal/sigset.h

long sys_sigprocmask (int how, const sigset_t *set, sigset_t *oldset) {
  const int ssz = sizeof (sigset_t);
  return syscall4 (NR_rt_sigprocmask, how, (long) set, (long) oldset, ssz);
}
