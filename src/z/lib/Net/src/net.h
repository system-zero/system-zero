#ifndef NET_HDR
#define NET_HDR

#define MAXLEN_ERROR_MSG 255
#define MAXLEN_STATUS_MSG 4096
#define MAXLEN_HEADER_MSG MAXLEN_STATUS_MSG
#define MAXLEN_STATUS_SSL_MSG 3000
#define MAXLEN_HEADER_SSL_MSG MAXLEN_STATUS_SSL_MSG

#define NET_VERBOSE_LEVEL_ONE   1
#define NET_VERBOSE_LEVEL_TWO   2
#define NET_VERBOSE_LEVEL_THREE 3

typedef struct net_t net_t;

typedef int (*NetOutputCallback) (net_t *, FILE *, char *, size_t, size_t, size_t);

typedef struct netOptions {
  const char *outputFile;
  int onlyParseURL;
  int outputToFile;
  NetOutputCallback outputCallback;
  int outputToCallback;
  int verbose;
} netOptions;

#define NetOptions(...) (netOptions) { \
  .outputFile = NULL,                  \
  .onlyParseURL = 0,                   \
  .outputToFile = 1,                   \
  .outputCallback = NULL,              \
  .outputToCallback = 0,               \
  .verbose = 0,                        \
  __VA_ARGS__                          \
}

typedef struct net_set_self {
  void (*outputCallback) (net_t *, NetOutputCallback);
} net_set_self;

typedef struct net_get_self {
  int (*statusCode) (net_t *);
  const char *(*statusCodeAsString) (net_t *);
  char *(*errorMsg) (net_t *);
} net_get_self;

typedef struct net_self {
  net_get_self get;
  net_set_self set;

  net_t *(*new) (netOptions);
  void (*release) (net_t **);
  int (*fetch) (net_t *, char *);
} net_self;

typedef struct net_T {
  net_self self;
} net_T;

public net_T __init_net__ (void);
public void __deinit_net__ (net_T **);

#endif /* NET_HDR */
