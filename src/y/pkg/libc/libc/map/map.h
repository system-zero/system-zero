typedef struct map_type map_type;

typedef void  (*MapReleaseCb) (map_type *);
typedef void  (*MapSetValueCb) (void **);
typedef void *(*MapCopyCb) (void *, void *);

struct map_type {
  char *key;
  void *value;
  MapReleaseCb release;
  map_type *next;
};

typedef struct {
  map_type **slots;
  uint32_t num_slots;
  uint32_t num_keys;
} Map_Type;
