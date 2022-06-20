// provides: int isascii (int)

/* diet and neat implementation */
int isascii (int c) {
  return (unsigned int) c < 128u;
}
