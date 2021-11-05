#define LIBRARY "Base64"

#define REQUIRE_BASE64_TYPE  DONOT_DECLARE

#include <z/cenv.h>
          /* https://github.com/elzoughby/Base64 */
    /***********************************************************
    * Base64 library implementation                            *
    * @author Ahmed Elzoughby                                  *
    * @date July 23, 2017                                      *
    ***********************************************************/
         /* MANY THANKS */

static char base46_map[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
  't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', '+', '/'
};

static char *base64_encode (const char *plain) {
  size_t len = bytelen (plain);
  int counts = 0;
  uchar buffer[3];  // CHANGE [ag]: ->from char to unsigned (this handles multibytes)
  uchar *cipher = Alloc (len * 4 / 3 + 4);
  int i = 0, c = 0;

  for (i = 0; plain[i] != '\0'; i++) {
    buffer[counts++] = plain[i];
    if (counts == 3) {
      cipher[c++] = base46_map[buffer[0] >> 2];
      cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
      cipher[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
      cipher[c++] = base46_map[buffer[2] & 0x3f];
      counts = 0;
    }
  }

  if (counts > 0) {
    cipher[c++] = base46_map[buffer[0] >> 2];
    if (counts == 1) {
      cipher[c++] = base46_map[(buffer[0] & 0x03) << 4];
      cipher[c++] = '=';
    } else { // if counts == 2
      cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
      cipher[c++] = base46_map[(buffer[1] & 0x0f) << 2];
    }
    cipher[c++] = '=';
  }

  cipher[c] = '\0';   /* string padding character */
  return (char *) cipher;
}

static char *base64_decode (const char *cipher) {
  size_t len = bytelen (cipher);
  int counts = 0;
  uchar buffer[4]; /* CHANGE [ag]: same with above but looks the same eitherway, probably
                    * because cipher composition is limited to the ascii range
                    */
  uchar* plain = Alloc (len * 3 / 4);
  int i = 0, p = 0;

  for (i = 0; cipher[i] != '\0'; i++) {
    int k;
    for (k = 0 ; k < 64 && base46_map[k] != cipher[i]; k++);
    buffer[counts++] = k;
    if (counts == 4) {
      plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
      if (buffer[2] != 64)
        plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
      if (buffer[3] != 64)
        plain[p++] = (buffer[2] << 6) + buffer[3];
      counts = 0;
    }
  }

  plain[p] = '\0';
  return (char *) plain;
}

public base64_T __init_base64__ (void) {
  return (base64_T) {
    .self = (base64_self) {
      .encode = base64_encode,
      .decode = base64_decode
    }
  };
}
