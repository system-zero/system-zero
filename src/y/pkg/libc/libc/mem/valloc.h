#define VMEM_ALIGN_SIZE PAGESIZE

typedef struct __attribute__((packed)) {
  size_t size;
} VmemChunk;

#define VMEM_HEADER_SIZE (sizeof (VmemChunk))
