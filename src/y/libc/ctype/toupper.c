// provides: int toupper (int)

/* diet and neat implementation */
int toupper (int c) {
  return (unsigned int) (c - 'a') < 26u ? c + ('A' - 'a') : c;
}
