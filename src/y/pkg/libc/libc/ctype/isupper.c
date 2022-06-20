// provides: int isupper (int)

/* diet and neat implementation */
int isupper (int c) {
  return (unsigned int) (c - 'A') < 26u;
}
