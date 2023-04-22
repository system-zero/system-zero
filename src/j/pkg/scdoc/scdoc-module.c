/* This revealed the following bug in logic:
 * When compiling modules that link against libraries that is using the
 * memory interface from own libc, and when the library allocates an object
 * (like string *) and returns it to the module, and the module reuses this
 * object to return back to the caller, the language (that has been compiled
 * against the standard memory interface from standard libc), it will send 
 * this chunk of memory to standard free (), that it doesn't know how to handle
 * it, as there is no compatibility.
 */

/* so avoid libc functions and duplicate the output string to scdoc_parse()
 */

#define REQUIRE_STDIO
//#define REQUIRE_ALLOC
#define REQUIRE_STD_MODULE
//#define REQUIRE_BYTELEN
//#define REQUIRE_ISDIGIT
//#define REQUIRE_TOUPPER
//#define REQUIRE_Z_ENV

#include <z/cenv.h>
//#include <libc.h>
#include <scdoc.h>

MODULE(scdoc);

static VALUE _scdoc_parse (la_t *this, VALUE v_input) {
  ifnot (IS_STRING(v_input)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  string *input = AS_STRING(v_input);

  ScdocParser s;
  string *r = scdoc_parse (&s, input->bytes, input->num_bytes);

  if (r) r = String.dup (r);

  scdoc_release (&s);

  return (r isnot NULL ? STRING(r) : NULL_VALUE);
}

public int __init_scdoc_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "scdoc_parse" , PTR(_scdoc_parse), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Scdoc = {
      parse : scdoc_parse,
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_scdoc_module__ (la_t *this) {
  (void) this;
}
