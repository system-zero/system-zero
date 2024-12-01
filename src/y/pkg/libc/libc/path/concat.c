// as: path_concat
// provides: char *path_concat (const char *, size_t, const char *, size_t)
// comment: assume Alloc

char *path_concat (const char *a, size_t a_sz, const char *b, size_t b_sz) {
  char *spb = (char *) b;
  while (*spb == DIR_SEP) {
    spb++;
    b_sz--;
  }

  if (b_sz) {
    char *sp = (char *) a + (a_sz - 1);
    while (*sp == DIR_SEP) {
      sp--;
      a_sz--;
    }
  }

  char *new = Alloc (a_sz + b_sz + (b_sz != 0) + 1);
  *new = '\0';

  size_t i = 0;
  for (; i < a_sz; i++)
    new[i] = a[i];

  if (b_sz) {
    new[i++] = DIR_SEP;

    for (; i < a_sz + b_sz + 1; i++)
      new[i] = *spb++;
  } else {
    while (new[i - 1] == DIR_SEP)
      if (new[i - 2] == DIR_SEP)
        i--;
      else
        break;
  }

  new[i] = '\0';
  return new;
}
