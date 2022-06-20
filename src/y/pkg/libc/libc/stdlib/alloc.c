// provides: void *malloc (size_t)
// provides: void *calloc (size_t, size_t)
// provides: void free (void *)
// provides: void *realloc (void *, size_t)
// requires: sys/mman.c
// requires: string/memset.c
// requires: string/memcpy.c

#define PGSIZE    4096
#define PGMASK    (PGSIZE - 1)
#define MSETMAX    4096
#define MSETLEN    (1 << 15)

struct mset {
  int refs;
  int size;
};

static struct mset *pool;

static int mk_pool (void) {
  if (pool && !pool->refs) {
    pool->size = sizeof (*pool);
    return 0;
  }

  pool = mmap (NULL, MSETLEN, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  if (pool == MAP_FAILED) {
    pool = NULL;
    return 1;
  }

  pool->size = sizeof (*pool);
  pool->refs = 0;
  return 0;
}

void *malloc (size_t n) {
  void *m;

  if (n >= MSETMAX) {
    m = mmap (NULL, n + PGSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (m == MAP_FAILED)
      return NULL;

    *(long *) m = n + PGSIZE;  /* store length in the first page */
    return (long *) m + PGSIZE;
  }

  if (!pool || (size_t) (MSETLEN - pool->size) < n + sizeof (void *))
    if (mk_pool())
      return NULL;

  m = (void *) ((long *) pool + pool->size);
  *(void **) m = pool;      /* the address of the owning mset */
  pool->refs++;
  pool->size += (n + sizeof (void *) + 7) & ~7;

  if (!((unsigned long) (pool + pool->size + sizeof(void *)) & PGMASK))
    pool->size += sizeof (long);

  return (long *) m + sizeof (void *);
}

void *calloc (size_t n, size_t sz) {
  void *r = malloc (n * sz);
  if (r)
    memset (r, 0, n * sz);

  return r;
}

void free (void *v) {
  if (!v)
    return;

  if ((unsigned long) v & PGMASK) {
    struct mset *mset = *(void **) ((long *) v - sizeof(void *));
    mset->refs--;
    if (!mset->refs && mset != pool)
      munmap (mset, mset->size);
  } else {
    munmap ((long *) v - PGSIZE, *(long *) ((long *) v - PGSIZE));
  }
}

void *realloc (void *v, size_t size) {
  if (NULL == v) {
    if (size > 0)
      return malloc (size);

    return v;
  }

  if (size == 0) {
    free (v);
    return NULL;
  }

  size_t sz = size;
  if ((unsigned long) v & PGMASK) {
    struct mset *mset = *(void **) ((long *) v - sizeof (void *));
    if (mset != pool)
      sz += mset->size;
  } else {
    sz += *(long *) ((long *) v - PGSIZE);
  }

  void *new = malloc (size);
  memcpy (new, v, size);
  free (v);
  return new;
}
