typedef struct __attribute__((packed)) {
  uint chunk_size;
  uint prev_chunk_size;
  uint user_flags;
  short unused;
  char is_used;
  char pointer[1];
} MemChunk;

typedef void *(*MemAlloc) (uint);

#define HEADER_SIZE (sizeof (MemChunk))
#define MEM_ALIGN_SIZE 16

#ifndef SPLIT_WHEN_IS_AT_LEAST
#define SPLIT_WHEN_IS_AT_LEAST 64
#endif

#ifndef INCREASE_EXTRA_BYTES
#define INCREASE_EXTRA_BYTES   1024
#endif

#define Alloc   mem_calloc
#define Realloc mem_realloc
#define Release(__ptr__) mem_release ((void **) &__ptr__)
