// provides: size_t str_byte_copy (char *, const char *, size_t)
// comment: Unsafe as it doesn't care if the requested bytes don't fit in destination and
// comment: it never appends the NUL byte terminator.
// comment: It just does what others ask to do, and it does it probably without mistake.

size_t str_byte_copy (char *dest, const char *src, size_t nbytes) {
  const char *sp = src;
  size_t len = 0;

  while (len < nbytes && *sp) {
    dest[len] = *sp++;
    len++;
  }

  return len;
}
