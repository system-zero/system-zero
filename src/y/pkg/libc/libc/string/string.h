#define MIN_STRING_SIZE 8

typedef struct string {
    char *bytes;
  size_t num_bytes;
  size_t mem_size;
} string;
