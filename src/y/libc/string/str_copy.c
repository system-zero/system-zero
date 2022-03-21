// provides: size_t str_copy (char *, size_t, const char *, size_t)
// provides: #define strncpy(_d_,_sd_,_s_,_sz_) str_copy(_d_,_s_,_sz_)
// requires: string/str_byte_copy.c

size_t str_copy (char *dest, size_t dest_len, const char *src, size_t nelem) {
  size_t num = (nelem > (dest_len - 1) ? dest_len - 1 : nelem);
  size_t len = (NULL == src ? 0 : str_byte_copy (dest, src, num));
  dest[len] = '\0';
  return len;
}
