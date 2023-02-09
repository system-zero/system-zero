// requires: signal/sigset.h

typedef void (*sighandler_t)(int);

struct sigaction {
  union {
    void (*action)(int, void*, void*);
    sighandler_t sa_handler;
  };

  unsigned long sa_flags;
  void (*restorer)(void);
  sigset_t sa_mask;
};
