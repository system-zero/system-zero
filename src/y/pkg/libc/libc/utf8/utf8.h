/* this code belongs to? */
const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};
/* the only reference found from the last research,
 * was at the julia programming language sources,
 * (this code and the functions that make use of it,
 * is atleast 12 years old, lying (during a non network season))
 */

typedef struct {
  char buf[5];
  int len;
  int width;
  utf8 code;
} utf8_char;


#ifndef TO_LOWER
#define TO_LOWER 0
#endif

#ifndef TO_UPPER
#define TO_UPPER 1
#endif
