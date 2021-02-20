#ifndef ARGPARSE_HDR
#define ARGPARSE_HDR

typedef struct argparse_t argparse_t;
typedef struct argparse_option_t argparse_option_t;

typedef int argparse_callback (argparse_t *, const argparse_option_t *);

enum argparse_flag {
  ARGPARSE_STOP_AT_NON_OPTION = (1 << 0),
  SHORT_OPT_HAS_NO_DASH = (1 << 1),
  ARGPARSE_DONOT_EXIT_ON_UNKNOWN = (1 << 2)
};

enum argparse_option_type {
  ARGPARSE_OPT_END,
  ARGPARSE_OPT_GROUP,
  ARGPARSE_OPT_BOOLEAN,
  ARGPARSE_OPT_BIT,
  ARGPARSE_OPT_INTEGER,
  ARGPARSE_OPT_FLOAT,
  ARGPARSE_OPT_STRING,
};

enum argparse_option_flags {
  OPT_NONEG = 1,
};

#define OPT_END()        { ARGPARSE_OPT_END, 0, NULL, NULL, 0, NULL, 0, 0 }
#define OPT_BOOLEAN(...) { ARGPARSE_OPT_BOOLEAN, __VA_ARGS__ }
#define OPT_BIT(...)     { ARGPARSE_OPT_BIT, __VA_ARGS__ }
#define OPT_INTEGER(...) { ARGPARSE_OPT_INTEGER, __VA_ARGS__ }
#define OPT_FLOAT(...)   { ARGPARSE_OPT_FLOAT, __VA_ARGS__ }
#define OPT_STRING(...)  { ARGPARSE_OPT_STRING, __VA_ARGS__ }
#define OPT_GROUP(h)     { ARGPARSE_OPT_GROUP, 0, NULL, NULL, h, NULL, 0, 0 }
#define OPT_HELP()       OPT_BOOLEAN('h', "help", NULL,                 \
                                     "show this help message and exit", \
                                     argparse_help_cb, 0, OPT_NONEG)

struct argparse_option_t {
  enum argparse_option_type type;
  const char short_name;
  const char *long_name;
  void *value;
  const char *help;
  argparse_callback *callback;
  intptr_t data;
  int flags;
};

struct argparse_t {
  const char
    *const *usages,
    *description,
    *epilog,
    **argv,
    **out,
    *optvalue;

  int
    argc,
    flags,
    cpidx;

  const argparse_option_t *options;
};

typedef struct argparse_self {
  int
    (*init) (argparse_t *, argparse_option_t *, const char *const *, int),
    (*exec) (argparse_t *, int, const char **);

  void
    (*print_usage) (argparse_t *);

} argparse_self;

typedef struct argparse_T {
  argparse_self self;
} argparse_T;

public int argparse_help_cb (argparse_t *, const argparse_option_t *);
public argparse_T __init_argparse__ (void);

#endif /* ARGPARSE_HDR */
