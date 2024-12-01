// provides: int ev_watch (Ev *)
// provides: int ev_add_event (Ev *, int, uint32_t, EventFun[NUM_EVENTS], void *)
// provides: int ev_remove_event (Ev *, int)
// provides: int ev_new_state (Ev *, uint)
// provides: void ev_release_state (Ev *)
// requires: stdlib/alloc.c
// requires: unistd/close.c
// requires: unistd/fcntl.h
// requires: sys/event.h
// requires: sys/event.c
// requires: sys/ev.h

int ev_add_event (Ev *ev, int fd, uint32_t events, EventFun fun[NUM_EVENTS], void *client_data) {
  struct epoll_event event = (struct epoll_event) {
    .data.fd = fd,
    .events = events
  };

  sys_epoll_ctl (ev->epfd, EPOLL_CTL_ADD, fd, &event);

  uint i;
  for (i = 0; i < ev->num_events; i++)
    if (ev->events[i]->fd == -1)
      break;

  // handle i == ev->num_events

  event_init (ev->events[i], fd, fun, client_data);

  return 0;
}

int ev_remove_event (Ev *ev, int fd) {
  uint i;
  for (i = 0; i < ev->num_events; i++) {
    event_t *event = ev->events[i];
    if (fd == event->fd) {
      event_init (event, -1, NULL, NULL);
      break;
    }
  }

  // handle i == num_events

  sys_epoll_ctl (ev->epfd, EPOLL_CTL_DEL, fd, NULL);
  sys_close (fd);

  return 0;
}

int ev_watch (Ev *ev) {
  struct epoll_event events[ev->num_events];
  sigset_t *sigmask = NULL;
  event_t *event;
  int retval = 0;

  for (;;) {
  next:
    event = NULL;

    int nfds = sys_epoll_wait (ev->epfd, events, ev->num_events, -1, sigmask);

    if (-1 == nfds) {
      tostderr ("%s\n", errno_string (sys_errno));
      retval = -1;
      break;
    }

    for (int i = 0; i < nfds; i++) {
      for (uint j = 0; j < ev->num_events; j++) {
        if (events[i].data.fd == ev->events[j]->fd) {
          event = ev->events[j];
          break;
        }
      }

      if (event == NULL)
        continue;

      if (events[i].events & EPOLLIN) {
        EventFun fun = event->fun[READ_EVENT];
        fun (event); // handle retval
        goto next;
     }

      if (events[i].events & EPOLLOUT) {
        EventFun fun = event->fun[WRITE_EVENT];
        fun (event); // handle retval
        goto next;
      }

      if (events[i].events & EPOLLHUP) {
        ev_remove_event (ev, events[i].data.fd);
        goto theend;
      }

      tostderr ("unexpected event %d\n", events[i].events);
      retval = -1;
      goto theend;
    }
  }

theend:
  return retval;
}

void ev_release_state (Ev *ev) {
  if (ev->num_events) {
    for (uint i = 0; i < ev->num_events; i++)
      Release (ev->events[i]);

    Release (ev->events);

    ev->num_events = 0;
  }

  if (ev->epfd != -1) {
    sys_close (ev->epfd);
    ev->epfd = -1;
  }
}

int ev_new_state (Ev *ev, uint num_events) {
  // handle ev->epfd != -1

  int epfd = sys_epoll_create (1);
  // handle failure

  sys_fcntl3 (epfd, F_SETFD, FD_CLOEXEC);

  ev->epfd = epfd;

  // hendle ev->events != NULL

  ev->events = Alloc (sizeof (event_t *) * num_events);

  uint i;
  for (i = 0; i < num_events + 1; i++) {
    event_t *event = Alloc (sizeof (event_t));
    event_init (event, -1, NULL, NULL);
    ev->events[i] = event;
  }

  ev->events[i] = NULL;

  ev->num_events = num_events;

  return 0;
}
