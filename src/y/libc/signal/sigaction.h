// requires: signal/sigset.h

struct sigaction {
  union {
    void (*action)(int, void*, void*);
    void (*handler)(int);
  };

  unsigned long flags;
  void (*restorer)(void);
  struct sigset mask;
};
