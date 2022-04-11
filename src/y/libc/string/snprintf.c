// provides: int snprintf (char *, size_t, const char *, ...) __attribute__((format(printf, 3, 0)))
// requires: string/vsnprintf.c

 __attribute__((format(printf, 3, 0)))
int snprintf (char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vsnprintf (str, size, fmt, ap);
    va_end(ap);

    return ret;
}