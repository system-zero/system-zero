// provides: int ustring_charlen (uchar)

int ustring_charlen (uchar c) {
  if (c < 0x80) return 1;
  if ((c & 0xe0) is 0xc0) return 2;
  return 3 + ((c & 0xf0) isnot 0xe0);
}
