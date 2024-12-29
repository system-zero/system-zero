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

#define Alloc   mem_calloc
#define Realloc mem_realloc
#define Release(__ptr__) mem_release ((void **) &__ptr__)
//#define Release(__ptr__) mem_mark_unused (__ptr__)
