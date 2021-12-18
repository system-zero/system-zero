#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_SMAP_TYPE     DONOT_DECLARE
#define REQUIRE_MAP_MACROS

#include <z/cenv.h>

struct smap_t {
  char *key;
  string_t *value;
  smap_t *next;
};

struct Smap_t {
  smap_t **slots;
  size_t
    num_slots,
    num_keys;
};

static void smap_release_slot (smap_t *item) {
  MAP_RELEASE_SLOT(item, smap_t, String.release);
}

static void smap_clear (Smap_t *map) {
  MAP_CLEAR(map, smap_release_slot);
}

static void smap_release (Smap_t *map) {
  if (NULL is map) return;
  MAP_RELEASE(map, smap_clear);
  map = NULL;
}

static Smap_t *smap_new (int num_slots) {
  return MAP_NEW(Smap_t, smap_t, num_slots);
}

static string_t *smap_get (Smap_t *smap, const char *key) {
  uint idx = 0;
  smap_t *item = MAP_GET(smap_t, smap, key, idx);
  ifnot (NULL is item) return item->value;
  return NULL;
}

static int smap_set (Smap_t *smap, const char *key, string_t *value) {
  string_t *old = smap_get (smap, key);

  MAP_SET(smap_t, smap, key, value);

  ifnot (NULL is old)
    String.release (old);

  return OK;
}

static int smap_key_exists (Smap_t *smap, const char *key) {
  uint idx = 0;
  smap_t *item = MAP_GET(smap_t, smap, key, idx);
  return (NULL isnot item);
}

public smap_T __init_smap__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);

  return (smap_T) {
    .self = (smap_self) {
      .new = smap_new,
      .release = smap_release,
      .clear = smap_clear,
      .get = smap_get,
      .set = smap_set,
      .key_exists = smap_key_exists
    }
  };
}
