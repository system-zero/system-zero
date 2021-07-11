#define REQUIRE_STDIO

#define REQUIRE_VMAP_TYPE     DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE map_set (la_t *this, VALUE v_map, VALUE v_key, VALUE v_val) {
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  int retval = La.map.reset_value (this, map, key, v_val);
  return INT(retval);
}

static VALUE map_get (la_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);
  VALUE *v = (VALUE *) Vmap.get (map, key);
  return (*v);
}

static VALUE map_remove (la_t *this, VALUE v_map, VALUE v_key) {
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  VALUE *val = Vmap.pop (map, key);
  if (val isnot NULL)
    La.map.release_value (this, val);

  return INT(LA_OK);
}

static VALUE map_key_exists (la_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  return INT(Vmap.key_exists (map, key));
}

static VALUE map_keys (la_t *this, VALUE v_map) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  Vmap_t *map = AS_MAP(v_map);
  int num = Vmap.num_keys (map);
  ArrayType *v_keys = Alloc (sizeof (ArrayType));
  v_keys->type = STRING_TYPE;
  v_keys->len  = num;
  string **keys = Vmap.keys (map);
  v_keys->value = ARRAY(keys);
  return ARRAY(v_keys);
}

static VALUE array_where (la_t *this, VALUE v_array, VALUE v_expr) {
  ifnot (IS_ARRAY(v_array)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");
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

  integer *r_ar = Alloc (sizeof (integer));
  integer len = 0;

  switch (type) {
    case STRING_TYPE: {
      char *expr = AS_STRING_BYTES(v_expr);
      string **s_ar = (string **) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (Cstring.eq (s_ar[i]->bytes, expr)) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }

    case INTEGER_TYPE: {
      int expr = AS_INT(v_expr);
      integer *i_ar = (integer *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (i_ar[i] is expr) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }

    case NUMBER_TYPE: {
      double expr = AS_NUMBER(v_expr);
      double *d_ar = (double *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (d_ar[i] is expr) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }
  }

  ArrayType *res_array = Alloc (sizeof (ArrayType));
  res_array->type = INTEGER_TYPE;
  res_array->len  = len;
  res_array->value = ARRAY(r_ar);
  return ARRAY(res_array);
}

#define EvalString(...) #__VA_ARGS__

public int __init_std_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(vmap);
  __INIT__(cstring);

  (void) stringType;

  LaDefCFun lafuns[] = {
    { "map_set",         PTR(map_set), 3 },
    { "map_get",         PTR(map_get), 2 },
    { "map_keys",        PTR(map_keys), 1 },
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
       "get" : map_get,
       "keys" : map_keys,
       "remove" : map_remove,
       "key_exists" : map_key_exists
     };

    public var Array = {
      "where" : array_where
    };

    public var Std = null
 );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_std_module__ (la_t *this) {
  (void) this;
  return;
}
