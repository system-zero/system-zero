#define DECIMAL_NUM_DIGITS 63

typedef struct decimal_t {
  char digits[DECIMAL_NUM_DIGITS + 1];
  size_t size;
  int base;
} decimal_t;
