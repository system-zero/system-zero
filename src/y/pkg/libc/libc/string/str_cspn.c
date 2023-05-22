// provides: size_t str_cspn (const char *, const char *)

size_t str_cspn (const char *s, const char *reject) {
  size_t len = 0;

  const char *spa = s;

  for (; *spa; spa++) {
    const char *spb = reject;
    for (; *spb; spb++) {
      if (*spa == *spb)
        return len;
    }

    len++;
  }

  return len;
}
