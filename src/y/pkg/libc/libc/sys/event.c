// provides: event_t *event_init (event_t *, int, EventFun[NUM_EVENTS], void *)
// provides: int event_register_fun (event_t *, int, EventFun)
// requires: sys/event.h

static int event_void_fun (event_t *event) {
  (void) event;
  return 0;
}

int event_register_fun (event_t *event, int idx, EventFun fun) {
  // handle out of bounds

  if (NULL == fun)
    event->fun[idx] = event_void_fun;
  else
    event->fun[idx] = fun;

  return 0;
}

event_t *event_init (event_t *event, int fd, EventFun fun[NUM_EVENTS], void *client_data) {
  event->fd = fd;
  event->client_data = client_data;

  if (NULL == fun) {
    for (int idx = 0; idx < NUM_EVENTS; idx++)
      event_register_fun (event, idx, NULL);
  } else {
    for (int idx = 0; idx < NUM_EVENTS; idx++)
      event_register_fun (event, idx, fun[idx]);
  }

  return event;
}
