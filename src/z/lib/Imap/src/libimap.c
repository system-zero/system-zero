#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_IMAP_TYPE     DONOT_DECLARE
#define REQUIRE_MAP_MACROS

#include <z/cenv.h>

struct imap_t {
  char *key;
  int   value;
  imap_t *next;
};

struct Imap_t {
  imap_t **slots;
  size_t
    num_slots,
    num_keys;
};

static void imap_release_slot (imap_t *item) {
  MAP_RELEASE_SLOT(item, imap_t, (void));
}

static void imap_clear (Imap_t *map) {
  MAP_CLEAR(map, imap_release_slot);
}

static void imap_release (Imap_t *map) {
  MAP_RELEASE(map, imap_clear);
}

static Imap_t *imap_new (int num_slots) {
  return MAP_NEW(Imap_t, imap_t, num_slots);
}

static int imap_get (Imap_t *imap, char *key) {
  uint idx = 0;
  imap_t *item = MAP_GET(imap_t, imap, key, idx);
  ifnot (NULL is item) return item->value;
  return 0;
}

static int imap_set (Imap_t *imap, char *key, int val) {
  imap_t *m = MAP_SET(imap_t, imap, key, val);
  return m->value;
}

static int imap_set_with_keylen (Imap_t *imap, char *key) {
  imap_t *m = MAP_SET(imap_t, imap, key, bytelen (key));
  return m->value;
}

static int imap_set_by_callback (Imap_t *imap, char *key, ImapSetValue_cb cb) {
  imap_t *m = MAP_NEW_ITEM(imap_t, imap, key);
  cb (&m->value);
  return m->value;
}

static int imap_key_exists (Imap_t *imap, char *key) {
  uint idx = 0;
  imap_t *item = MAP_GET(imap_t, imap, key, idx);
  return (NULL isnot item);
}

public imap_T __init_imap__ (void) {
  __INIT__ (cstring);

  return (imap_T) {
    .self = (imap_self) {
      .new = imap_new,
      .get = imap_get,
      .set = imap_set,
      .clear = imap_clear,
      .release = imap_release,
      .key_exists = imap_key_exists,
      .set_by_callback = imap_set_by_callback,
      .set_with_keylen = imap_set_with_keylen
    }
  };
}
