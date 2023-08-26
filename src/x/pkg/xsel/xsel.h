#ifndef XSEL_H
#define XSEL_H

#define XSEL_CLIPBOARD  1
#define XSEL_PRIMARY    2

typedef struct {
  int selection;
  long timeout;
  int daemonize;
  int append_selection;
  int rm_last_nl;
} xsel_opts_t;

#define XselOpts(...) (xsel_opts_t) { \
  .selection = XSEL_PRIMARY,          \
  .timeout = 0,                       \
  .daemonize = 1,                     \
  .append_selection = 0,              \
  .rm_last_nl = 0,                    \
  __VA_ARGS__ }

char *xsel_get (xsel_opts_t);
int   xsel_put (char *, xsel_opts_t);
void deinit_xsel (void);

#endif /* XSEL_H */
