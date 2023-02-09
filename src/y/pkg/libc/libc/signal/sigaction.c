// provides: long sigaction (int, const struct sigaction *, struct sigaction *)
// requires: signal/sigaction.h

long sigaction (int signum, const struct sigaction *act, struct sigaction *oldact) {
  const int ssz = sizeof (sigset_t);
  return syscall4 (NR_rt_sigaction, signum, (long) act, (long) oldact, ssz);
}
