#define READ_EVENT   0
#define WRITE_EVENT  1

#define NUM_EVENTS WRITE_EVENT + 1

typedef struct event_t event_t;

typedef int (*EventFun) (event_t *);

struct event_t {
  int fd;
  EventFun fun[2];
  void *client_data;
};
