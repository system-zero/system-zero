#define REQUIRE_STDIO

#define REQUIRE_VMAP_TYPE     DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE map_set (la_t *this, VALUE v_map, VALUE v_key, VALUE v_val) {
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  La.map.reset_value (this, map, key, v_val);
  return INT(LA_OK);
}

static VALUE map_remove (la_t *this, VALUE v_map, VALUE v_key) {
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  VALUE *val = Vmap.pop (map, key);
  if (val isnot NULL)
    La.map.release_value (this, val);

  return INT(LA_OK);
}

static VALUE map_key_exists (la_t *this, VALUE v_map, VALUE v_key) {
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  return INT(Vmap.key_exists (map, key));
}

static VALUE array_where (la_t *this, VALUE v_array, VALUE v_expr) {
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  int type = array->type;
  if (type isnot v_expr.type) {
    La.set.CFuncError (this, LA_ERR_TYPE_MISMATCH);
    La.set.curMsg (this, STR_FMT("%s, expression is not the same type", __func__));
    return NULL_VALUE;
  }

  switch (type) {
    case STRING_TYPE:
    case INTEGER_TYPE:
    case NUMBER_TYPE:
      break;

    default:
      La.set.CFuncError (this, LA_ERR_TYPE_MISMATCH);
      La.set.curMsg (this, STR_FMT("%s, has been implemented for (Integer|String|Number)Type", __func__));
      return NULL_VALUE;
  }

  ArrayType *res_ar = ARRAY_NEW(INTEGER_TYPE, array->len);
  integer *r_ar = (integer *) AS_ARRAY(res_ar->value);

  switch (type) {
    case STRING_TYPE: {
      char *expr = AS_STRING_BYTES(v_expr);
      string **s_ar = (string **) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++)
        r_ar[i] = Cstring.eq (s_ar[i]->bytes, expr);
      break;
    }

    case INTEGER_TYPE: {
      int expr = AS_INT(v_expr);
      integer *i_ar = (integer *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++)
        r_ar[i] = i_ar[i] is expr;
      break;
    }

    case NUMBER_TYPE: {
      double expr = AS_NUMBER(v_expr);
      double *d_ar = (double *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++)
        r_ar[i] = d_ar[i] is expr;
      break;
    }
  }

  return ARRAY(res_ar);
}

#define EvalString(...) #__VA_ARGS__

public int __init_std_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(vmap);
  __INIT__(string);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "map_set",         PTR(map_set), 3 },
    { "map_remove",      PTR(map_remove), 2 },
    { "map_key_exists",  PTR(map_key_exists), 2 },
    { "array_where",     PTR(array_where), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Map = {
       "set" : map_set,
       "remove" : map_remove,
       "key_exists" : map_key_exists
     };

    public var Array = {
      "where" : array_where
    }
 );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_std_module__ (la_t *this) {
  (void) this;
  return;
}
