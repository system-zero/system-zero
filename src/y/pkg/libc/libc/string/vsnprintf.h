#ifndef STRING_FORMAT_SIZE
#define STRING_FORMAT_SIZE(__fmt__)                                   \
({                                                                    \
  int size_ = 0;                                                      \
  va_list ap; va_start(ap, __fmt__);                                  \
  size_ = sys_vsnprintf (NULL, 0, __fmt__, ap);                       \
  va_end(ap);                                                         \
  size_;                                                              \
})
#endif
