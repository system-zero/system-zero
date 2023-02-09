// provides: int sigaddset(sigset_t *, int)
// requires: signal/sigset.h

#define _NSIG     65
#define SIGRTMIN  32
#define SIGRTMAX  (_NSIG-1)

#define __sigmask(sig) ( ((unsigned long)1) << (((sig)-1) % (8*sizeof(unsigned long))) )
#define __sigword(sig) ( ((sig)-1) / (8*sizeof(unsigned long)) )

int sigaddset (sigset_t *set, int signo) {
  if ((signo < 1) || (signo > SIGRTMAX)) {
    sys_errno = EINVAL;
    return -1;
  } else {
    unsigned long __mask = __sigmask (signo);
    unsigned long __word = __sigword (signo);
    set->sig[__word] |= __mask;
    return 0;
  }
}
