#define REQUIRE_STDIO
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_MAP_MACROS

#include <z/cenv.h>

typedef struct Vmap_t {
  vmap_t **slots;
  size_t
    num_slots,
    num_keys;
} Vmap_t;

typedef struct vmap_t {
  char *key;
  void *val;
  int is_constant;
  VmapRelease_cb release;
  vmap_t *next;
} vmap_t;

static void vmap_release_slot (vmap_t *item) {
  while (item) {
    vmap_t *tmp = item->next;
    item->release (item->val);
    free (item->key);
    free (item);
    item = tmp;
  }
}

static void vmap_clear (Vmap_t *map) {
  MAP_CLEAR(map, vmap_release_slot);
}

static void vmap_release (Vmap_t *map) {
  MAP_RELEASE(map, vmap_clear);
}

static Vmap_t *vmap_new (int num_slots) {
  return MAP_NEW(Vmap_t, vmap_t, num_slots);
}

static void *vmap_pop (Vmap_t *vmap, char *key) {
  uint idx = 0;
  vmap_t *item = MAP_POP(vmap_t, vmap, key, idx);

  ifnot (NULL is item) {
    void *val = item->val;
    free (item->key);
    free (item);
    return val;
  }

  return NULL;
}

static void *vmap_get (Vmap_t *vmap, char *key) {
  uint idx = 0;
  vmap_t *item = MAP_GET(vmap_t, vmap, key, idx);
  ifnot (NULL is item) return item->val;
  return NULL;
}

static int vmap_set (Vmap_t *vmap, char *key, void *val, VmapRelease_cb cb, int is_constant) {
  if (NULL is cb) return NOTOK;

  uint idx = 0;

  vmap_t *old = MAP_GET(vmap_t, vmap, key, idx);

  ifnot (NULL is old) {
    if (old->is_constant)
      return NOTOK;
  }

  vmap_t *item = MAP_SET(vmap_t, vmap, key, val);
  item->release = cb;
  item->is_constant = is_constant;

  ifnot (NULL is old)
    old->release (old->val);

  return OK;
}

static int vmap_key_exists (Vmap_t *vmap, char *key) {
  uint idx = 0;
  vmap_t *item = MAP_GET(vmap_t, vmap, key, idx);
  return (NULL isnot item);
}

public vmap_T __init_vmap__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);

  return (vmap_T) {
    .self = (vmap_self) {
      .new = vmap_new,
      .get = vmap_get,
      .set = vmap_set,
      .pop = vmap_pop,
      .clear = vmap_clear,
      .release = vmap_release,
      .key_exists = vmap_key_exists
    }
  };
}
