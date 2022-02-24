// provides: int islower (int)

int islower (int c) {
  return (unsigned) c - 'a' < 26;
}
