// provides: long sys_sigprocmask (int, const struct sigset *, struct sigset *)
// requires: signal/sigset.h

long sys_sigprocmask (int how, const struct sigset *set, struct sigset *oldset) {
  const int ssz = sizeof (struct sigset);
  return syscall4 (NR_rt_sigprocmask, how, (long) set, (long) oldset, ssz);
}
