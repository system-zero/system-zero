// depends: stdio/EOF.h

int ungetc (int c, FILE *fp) {
  if (fp->back == EOF)
    fp->back = c;
  return fp->back;
}
