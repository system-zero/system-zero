// provides: Map_Type *map_new (uint32_t)
// provides: map_type *map_get (Map_Type *, const char *)
// provides: void *map_get_value (Map_Type *, const char *)
// provides: void *map_pop_value (Map_Type *, const char *)
// provides: void *map_set_by_callback (Map_Type *, char *, MapReleaseCb, MapSetValueCb)
// provides: int map_set (Map_Type *, char *, void *, MapReleaseCb)
// provides: int map_set_with_key_allocated (Map_Type *, char *, void *, MapReleaseCb)
// provides: int map_key_exists (Map_Type *, const char *)
// provides: Map_Type *map_clone (Map_Type *, MapCopyCb, void *)
// provides: void map_clear (Map_Type *)
// provides: void map_release (Map_Type **)
// provides: char **map_keys (Map_Type *)
// requires: stdlib/alloc.c
// requires: string/str_eq.c
// requires: string/str_new.c
// requires: string/str_hash.c
// requires: map/map.h

#define MAP_HASH_KEY_DJB2(_m_, _k_) (map_hash_key (_k_) % (_m_)->num_slots)
#define MAP_HASH_KEY_FNV_1A(_m_, _k_)  (str_hash (_k_) % (_m_)->num_slots)

#ifndef MAP_HASH_KEY
#define MAP_HASH_KEY MAP_HASH_KEY_FNV_1A
//#define MAP_HASH_KEY MAP_HASH_KEY_DJB2
#endif

static inline uint32_t map_hash_key (const char *s) {
  char *sp = (char *) s;
  uint32_t hash = 5381;

  while (*sp)
     hash = ((hash << 5) + hash) + *sp++; /* hash * 33 + c */
     //  hash = (hash * 33) ^ *sp++;

  return hash;
}

static void map_release_slot (map_type *it) {
  while (it) {
    map_type *tmp = it->next;

    if (NULL != it->release)
      it->release (it->value);

    Release (it->key);
    Release (it);
    it = tmp;
  }
}

void map_clear (Map_Type *m) {
  for (uint32_t i = 0; i < m->num_slots; i++) {
    if (NULL == m->slots[i])
      continue;

    map_release_slot (m->slots[i]);
    m->slots[i] = NULL;
  }

  m->num_keys = 0;
}

void map_release (Map_Type **m) {
  if (NULL == *m)
    return;

  map_clear (*m);
  Release ((*m)->slots);
  Release (*m);
  *m = NULL;
}

Map_Type *map_new (uint32_t num_slots) {
  Map_Type *new = Alloc (sizeof (Map_Type));
  size_t size = sizeof (map_type) * num_slots;
  new->slots = Alloc (size);

  for (uint32_t i = 0; i < num_slots; i++)
    new->slots[i] = NULL;

  new->num_keys = 0;
  new->num_slots = num_slots;
  return new;
}

static map_type *__map_get (Map_Type *m, const char *key, uint32_t hash) {
  map_type *it = m->slots[hash];

  while (it) {
    if (str_eq (it->key, key))
      return it;

    it = it->next;
  }

  return NULL;
}

map_type *map_get (Map_Type *m, const char *key) {
  return __map_get (m, key, MAP_HASH_KEY(m, key));
}

void *map_get_value (Map_Type *m, const char *key) {
  map_type *it =  __map_get (m, key, MAP_HASH_KEY(m, key));

  if (NULL == it)
    return NULL;

  return it->value;
}

static map_type *__map_set (Map_Type *m, char *key) {
  uint32_t hash = MAP_HASH_KEY(m, key);

  map_type *new = NULL;

  map_type *old = __map_get (m, key, hash);

  if (NULL != old) {
    if (NULL != old->release)
      old->release (old->value);

    return old;
  }

  new = Alloc (sizeof (map_type));
  new->next = m->slots[hash];
  new->key = str_new_with (key);
  new->value = 0;
  m->slots[hash] = new;
  m->num_keys++;

  return new;
}

int map_set (Map_Type *m, char *key, void *value, MapReleaseCb cb) {
  map_type *map = __map_set (m, key);
  map->value = value;
  map->release = cb;
  return 0;
}

int map_set_with_key_allocated (Map_Type *m, char *key, void *value, MapReleaseCb cb) {
  map_type *map = __map_set (m, key);
  map->value = value;
  map->release = cb;
  return 0;
}

void *map_set_by_callback (Map_Type *m, char *key, MapReleaseCb r_cb, MapSetValueCb s_cb) {
  map_type *map = __map_set (m, key);
  map->release = r_cb;
  map->value = s_cb (map->value);
  return map->value;
}

int map_key_exists (Map_Type *m, const char *key) {
  uint32_t hash = MAP_HASH_KEY(m, key);
  map_type *it = m->slots[hash];
  while (it) {
    if (str_eq (it->key, key))
      return 1;
    it = it->next;
  }

  return 0;
}

void *map_pop_value (Map_Type *m, const char *key) {
  uint32_t hash = MAP_HASH_KEY(m, key);

  map_type *it = m->slots[hash];
  map_type *ti = NULL;

  while (it) {
    if (str_eq (it->key, key)) {
      void *value = it->value;

      map_type *next = it->next;
      if (ti)
        ti->next = next;
      else {
        if (next)
          m->slots[hash] = next;
        else
          m->slots[hash] = NULL;
      }

      Release (it->key);
      Release (it);

      m->num_keys--;

      return value;
    }

    ti = it;
    it = it->next;
  }

  return NULL;
}

Map_Type *map_clone (Map_Type *m, MapCopyCb copy, void *object) {
  Map_Type *new = map_new (m->num_slots);

  for (size_t i = 0; i < m->num_slots; i++) {
    map_type *map = m->slots[i];
    while (map) {
      void *value = copy (map->value, object);
      map_set (new, map->key, value, map->release);
      map = map->next;
    }
  }

  return new;
}

char **map_keys (Map_Type *m) {
  char **keys = Alloc (sizeof (char *) * m->num_keys);

  int idx = 0;
  for (size_t i = 0; i < m->num_slots; i++) {
    map_type *map = m->slots[i];
    while (map) {
      keys[idx++] = str_new_with (map->key);
      map = map->next;
    }
  }

  return keys;
}
