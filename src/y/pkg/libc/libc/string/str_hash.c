// provides: uint32_t str_hash (const char *)
// provides: uint64_t str_hash_64 (const char *)
// provides: uint32_t str_hash_32 (const char *)
// comment: FNV-1a hashing algorithm: https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
// comment: short version offset_basis and prime as default, suitable for small strings (keys)
// comment: probably worths another version with precomputed length

#ifndef HASH_OFFSET_BASIS_64
#define HASH_OFFSET_BASIS_64 ((uint64_t) 0xcbf29ce484222325ULL)
#endif

#ifndef HASH_OFFSET_BASIS_32
#define HASH_OFFSET_BASIS_32 ((uint32_t) 0x811c9dc5)
#endif

#ifndef HASH_PRIME_64
#define HASH_PRIME_64 ((uint64_t) 0x00000100000001B3ULL)
#endif

#ifndef HASH_PRIME_32
#define HASH_PRIME_32 ((uint32_t) 0x01000193)
#endif

uint32_t str_hash_32 (const char *bytes) {
  uint32_t hash = HASH_OFFSET_BASIS_32;

  char *sp = (char *) bytes;

  while (*sp) {
    hash ^= (uint8_t) *sp++;
    hash *= HASH_PRIME_32;
  }

  return hash;
}

uint64_t str_hash_64 (const char *bytes) {
  uint64_t hash = HASH_OFFSET_BASIS_64;

  char *sp = (char *) bytes;

  while (*sp) {
    hash ^= (uint8_t) *sp++;
    hash *= HASH_PRIME_64;
  }

  return hash;
}

uint32_t str_hash (const char *bytes) { return str_hash_32 (bytes); }

