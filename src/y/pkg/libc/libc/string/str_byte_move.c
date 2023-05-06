// provides: size_t str_byte_move (char *, size_t, size_t, size_t, size_t)

size_t str_byte_move (char *str, size_t len, size_t to_idx,
                            size_t from_idx, size_t nelem) {
  if (from_idx == to_idx)
    return 0;

  while (to_idx + nelem > len)
    nelem--;

  size_t n = nelem;

  if (to_idx > from_idx) {
    char *sp = str + from_idx + nelem;
    char *dsp = str + to_idx + nelem;

    while (nelem--) *--dsp = *--sp;
    return (n - nelem) - 1;
  }

  while (from_idx + nelem > len)
    nelem--;

  n = nelem;

  char *sp = str + from_idx;
  char *dsp = str + to_idx;

  while (nelem) {
    if ('\0' == *sp) {  // stop at the first null byte
      *dsp = '\0';      // this differs in memmove()
      break;
    }

    *dsp++ = *sp++;
    nelem--;
  }

  return n - nelem;
}
