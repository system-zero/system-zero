// provides: long sys_epoll_create (long)
// provides: long sys_epoll_ctl (long, long, long, struct epoll_event *)
// provides: long sys_epoll_wait (long, struct epoll_event *, long, long, sigset_t *) 
// requires: signal/sigset.h
// requires: sys/epoll.h

long sys_epoll_create (long flags) {
  return syscall1 (NR_epoll_create, flags);
}

long sys_epoll_ctl (long epfd, long mode, long fd, struct epoll_event *ev) {
  return syscall4 (NR_epoll_ctl, epfd, mode, fd, (long) ev);
}

long sys_epoll_wait (long epfd, struct epoll_event *ev, long maxevents,
    long timeout, sigset_t *sigmask) {
  return syscall5 (NR_epoll_wait, epfd, (long) ev, maxevents, timeout, (long) sigmask);
}
