typedef struct __attribute__((packed)) {
  uint chunk_size;
  uint prev_chunk_size;
  char is_used;
  char unused_byte;
  int  unused_int;
  short unused_short;
} MemChunk;

typedef void *(*MemAlloc) (uint);

#define MEM_HEADER_SIZE (sizeof (MemChunk))
#define MEM_ALIGN_SIZE 16

#define Alloc   mem_alloc
#define Realloc mem_realloc
#define Calloc  mem_calloc
#define Release(__ptr__) mem_release ((void **) &__ptr__)
