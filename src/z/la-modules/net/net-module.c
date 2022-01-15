#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_NET_TYPE      DECLARE

#include <z/cenv.h>

MODULE(net)

static VALUE net_fetch (la_t *this, VALUE v_url) {
  ifnot (IS_STRING(v_url)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *url = AS_STRING_BYTES(v_url);

  int verbose = GET_OPT_VERBOSE();
  net_t *net = Net.new (NetOptions (.verbose = verbose));

  int retval = Net.fetch (net, url);

  Net.release (&net);

  return INT(retval);
}

#define EvalString(...) #__VA_ARGS__

public int __init_net_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__ (net);
  __INIT__ (path);

  LaDefCFun lafuns[] = {
    {"net_fetch", PTR(net_fetch), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Net = {
      fetch : net_fetch,
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_net_module__ (la_t *this) {
  (void) this;
  return;
}
