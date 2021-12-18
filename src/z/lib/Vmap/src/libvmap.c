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
  void *value;
  int is_constant;
  VmapRelease_cb release;
  vmap_t *next;
} vmap_t;

static void vmap_release_slot (vmap_t *item) {
  while (item) {
    vmap_t *tmp = item->next;
    item->release (item->value);
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

static void *vmap_pop (Vmap_t *vmap, const char *key) {
  uint idx = 0;
  vmap_t *item = MAP_POP(vmap_t, vmap, key, idx);

  ifnot (NULL is item) {
    void *value = item->value;
    free (item->key);
    free (item);
    return value;
  }

  return NULL;
}

static void *vmap_get (Vmap_t *vmap, const char *key) {
  uint idx = 0;
  vmap_t *item = MAP_GET(vmap_t, vmap, key, idx);
  ifnot (NULL is item) return item->value;
  return NULL;
}

static int vmap_set (Vmap_t *vmap, const char *key, void *value, VmapRelease_cb cb, int is_constant) {
  if (NULL is cb) return NOTOK;

  uint idx = 0;

  void *old_value = NULL;

  vmap_t *old = MAP_GET(vmap_t, vmap, key, idx);

  ifnot (NULL is old) {
    old_value = old->value;
    if (old->is_constant)
      return NOTOK;
  }

  vmap_t *item = MAP_SET(vmap_t, vmap, key, value);
  item->release = cb;
  item->is_constant = is_constant;

  ifnot (NULL is old)
    old->release (old_value);

  return OK;
}

static int vmap_key_exists (Vmap_t *vmap, const char *key) {
  uint idx = 0;
  vmap_t *item = MAP_GET(vmap_t, vmap, key, idx);
  return (NULL isnot item);
}

static Vmap_t *vmap_clone (Vmap_t *vmap, VmapCopy_cb copy, void *object) {
  Vmap_t *new = vmap_new (vmap->num_slots);

  for (size_t i = 0; i < vmap->num_slots; i++) {
    vmap_t *item = vmap->slots[i];
    while (item) {
      void *value = copy (item->value, object);
      vmap_set (new, item->key, value, item->release, item->is_constant);
      item = item->next;
    }
  }

  return new;
}

static char *vmap_key (Vmap_t *vmap, void *object) {
  if (NULL is vmap) return NULL;

  for (size_t i = 0; i < vmap->num_slots; i++) {
    vmap_t *item = vmap->slots[i];
    while (item) {
      if (item->value is object)  return item->key;
      item = item->next;
    }
  }

  return NULL;
}

static int vmap_num_keys (Vmap_t *vmap) {
  return (int) vmap->num_keys;
}

static size_t vmap_sizeof (void) {
  return sizeof (Vmap_t);
}

static string **vmap_keys (Vmap_t *vmap) {
  string **keys = Alloc (sizeof (string) * vmap->num_keys);
  int idx = 0;
  for (size_t i = 0; i < vmap->num_slots; i++) {
    vmap_t *item = vmap->slots[i];
    while (item) {
      keys[idx++] = String.new_with (item->key);
      item = item->next;
    }
  }

  return keys;
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
      .key = vmap_key,
      .keys = vmap_keys,
      .clear = vmap_clear,
      .clone = vmap_clone,
      .size_of = vmap_sizeof,
      .release = vmap_release,
      .num_keys = vmap_num_keys,
      .key_exists = vmap_key_exists
    }
  };
}
