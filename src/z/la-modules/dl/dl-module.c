#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_DL_TYPE

#include <z/cenv.h>

MODULE(dl)

static VALUE dl_dump_dependencies (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);

  dl_t dl;
  dl.verbose =  GET_OPT_VERBOSE_WITH(0);
  dl.path = 0;
  int retval = DlDumpDependencies (&dl, file);
  if (retval is NOTOK) return NULL_VALUE;

  Vmap_t *m = Vmap.new (16);

  ArrayType *array = ARRAY_NEW(STRING_TYPE, dl.num);
  string **ar = (string **) AS_ARRAY(array->value);

  for (int i = 0; i < dl.num; i++)
    String.replace_with (ar[i], dl.dependencies[i]);

  La.map.set_value (this, m, "dependencies", ARRAY(array), 1);
  La.map.set_value (this, m, "bits", INT(dl.bits), 1);
  La.map.set_value (this, m, "endian", INT(dl.endian), 1);

  DlRelease (&dl);

  return MAP(m);
}

#define EvalString(...) #__VA_ARGS__

public int __init_dl_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"dl_dump_dependencies", PTR(dl_dump_dependencies), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  if (La.def_std (this, "LITTLE_ENDIAN", INTEGER_TYPE, INT(LITTLE_ENDIAN), -1)) return LA_NOTOK;
  if (La.def_std (this, "BIG_ENDIAN", INTEGER_TYPE, INT(BIG_ENDIAN), -1)) return LA_NOTOK;

  const char evalString[] = EvalString (
    public var Dl = {
      dump_dependencies : dl_dump_dependencies
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_dl_module__ (la_t *this) {
  (void) this;
  return;
}
