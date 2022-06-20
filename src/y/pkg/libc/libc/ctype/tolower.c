// provides: int tolower (int)

/* diet and neat implementation */
int tolower (int c) {
  return (unsigned int) (c - 'A') < 26u ? c + ('a' - 'A') : c;
}
