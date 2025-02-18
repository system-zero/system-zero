typedef struct __attribute__((packed)) {
  uint chunk_size;
  uint prev_chunk_size;
} MemChunk;

typedef void *(*MemAlloc) (size_t);

#define MEM_HEADER_SIZE (sizeof (MemChunk))
#define MEM_ALIGN_SIZE 8

#define Alloc   mem_alloc
#define Realloc mem_realloc
#define Calloc  mem_calloc
#define Release(__ptr__) mem_release ((void **) &__ptr__)
