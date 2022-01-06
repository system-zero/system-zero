#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_URL_TYPE

#include <z/cenv.h>

MODULE(url)

static VALUE url_parse (la_t *this, VALUE v_url) {
  ifnot (IS_STRING(v_url)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *url = AS_STRING_BYTES(v_url);

  url_t *parsedURL = UrlParse (0x0, 0, 0, url);

  if (NULL is parsedURL)
    return NULL_VALUE;

  Vmap_t *m = Vmap.new (16);

  if (NULL isnot parsedURL->scheme)
    La.map.set_value (this, m, "scheme", STRING_NEW(parsedURL->scheme), 1);
  else
    La.map.set_value (this, m, "scheme", NULL_VALUE, 1);

  if (NULL isnot parsedURL->host)
    La.map.set_value (this, m, "host", STRING_NEW(parsedURL->host), 1);
  else
    La.map.set_value (this, m, "host", NULL_VALUE, 1);

  if (NULL isnot parsedURL->fragment)
    La.map.set_value (this, m, "fragment", STRING_NEW(parsedURL->fragment), 1);
  else
    La.map.set_value (this, m, "fragment", NULL_VALUE, 1);

  if (NULL isnot parsedURL->query)
    La.map.set_value (this, m, "query", STRING_NEW(parsedURL->query), 1);
  else
    La.map.set_value (this, m, "query", NULL_VALUE, 1);

  if (NULL isnot parsedURL->path)
    La.map.set_value (this, m, "path", STRING_NEW(parsedURL->path), 1);
  else
    La.map.set_value (this, m, "path", NULL_VALUE, 1);

  if (NULL isnot parsedURL->user)
    La.map.set_value (this, m, "user", STRING_NEW(parsedURL->user), 1);
  else
    La.map.set_value (this, m, "user", NULL_VALUE, 1);

  if (NULL isnot parsedURL->pass)
    La.map.set_value (this, m, "pass", STRING_NEW(parsedURL->pass), 1);
  else
    La.map.set_value (this, m, "pass", NULL_VALUE, 1);

  La.map.set_value (this, m, "port", INT(parsedURL->port), 1);

  free (parsedURL);

  return MAP(m);
}

#define EvalString(...) #__VA_ARGS__

public int __init_url_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"url_parse", PTR(url_parse), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Url = {
      parse : url_parse
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_url_module__ (la_t *this) {
  (void) this;
  return;
}
