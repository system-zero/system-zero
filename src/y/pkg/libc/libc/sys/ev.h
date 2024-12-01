// requires: sys/event.h

typedef struct Ev {
  int epfd;
  event_t **events;
  uint num_events;
  /*  int flags; e.g., what todo when epfd != -1 when create state */
} Ev;

#define EvNew(...) (Ev) { \
  .epfd = -1,             \
  .events = NULL,         \
  .num_events = 0,        \
  __VA_ARGS__ }
