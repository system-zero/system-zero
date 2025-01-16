typedef struct __attribute__((packed)) MemChunk {
  uint chunk_size;
  struct MemChunk *prev;
  struct MemChunk *next;
  char is_used;
  char unused_byte;
  short unused_short;
} MemChunk;

typedef void *(*MemAlloc) (uint);

#define MEM_HEADER_SIZE (sizeof (MemChunk))
#define MEM_ALIGN_SIZE 16

#define Alloc   mem_alloc
#define Realloc mem_realloc
#define Calloc  mem_calloc
#define Release(__ptr__) mem_release ((void **) &__ptr__)
