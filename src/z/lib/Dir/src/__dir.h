#define DIR_MSG(...)  do {  \
  if (opts.msg)     \
    opts.msg_cb (stdout, __VA_ARGS__);} while (0)

#define DIR_ERROR(...) do { \
  if (opts.err)     \
    opts.err_cb (stderr, __VA_ARGS__);} while (0)

#define DIR_WARN DIR_ERROR
