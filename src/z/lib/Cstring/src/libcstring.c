#define LIBRARY "Cstring"

#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_CSTRING_TYPE DONOT_DECLARE

#include <z/cenv.h>

static int cstring_eq (const char *sa, const char *sb) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;
  for (; *spa == *spb; spa++, spb++)
    if (*spa == 0) return 1;

  return 0;
}

static int cstring_cmp_n (const char *sa, const char *sb, size_t n) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;
  for (;n--; spa++, spb++) {
    if (*spa != *spb)
      return (*(uchar *) spa - *(uchar *) spb);

    if (*spa == 0) return 0;
  }

  return 0;
}

/* just for clarity */
static int cstring_eq_n  (const char *sa, const char *sb, size_t n) {
  return (0 == cstring_cmp_n (sa, sb, n));
}

static char *cstring_byte_in_str (const char *s, int c) {
  const char *sp = s;
  while (*sp != c) {
    if (*sp == 0) return NULL;
    sp++;
  }
  return (char *) sp;
}

static char *cstring_byte_null_in_str (const char *s) {
  return cstring_byte_in_str (s, 0);
}

static char *cstring_byte_in_str_r (const char *s, int c) {
  const char *sp = cstring_byte_null_in_str (s);
  if (sp == s) return NULL;
  while (*--sp != c)
    if (sp == s) return NULL;

  return (char *) sp;
}

/* the same function under a different name is 25-30 times slower */
char *strstr (const char *str, const char *substr) {
  while (*str != '\0') {
    if (*str == *substr) {
      const char *spa = str + 1;
      const char *spb = substr + 1;

      while (*spa && *spb){
        if (*spa != *spb)
          break;
        spa++; spb++;
      }

      if (*spb == '\0')
        return (char *) str;
    }

    str++;
  }

  return NULL;
}

static size_t cstring_byte_cp (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem and *sp) { // this differs in memcpy()
    dest[len] = *sp++;
    len++;
  }

  return len;
}

static size_t cstring_byte_cp_all (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem) {
    dest[len] = *sp++; // like memcpy (needed in one case in the code)
    len++;             // i'm not sure if it is wise
  }

  return len;
}

static size_t cstring_cat (char *dest, size_t dest_sz, const char *src) {
  char *dp = cstring_byte_null_in_str (dest);
  size_t len = dp - dest;
  size_t i = 0;

  // do not change it for *src - it is confirmed that doesn't provide the expected
  while (src[i] and i + len < dest_sz - 1) *dp++ = src[i++];
  *dp = '\0';
  return len + i;
}

static size_t cstring_byte_mv (char *str, size_t len, size_t to_idx,
                                   size_t from_idx, size_t nelem) {
  if (from_idx is to_idx) return 0;
  while (to_idx + nelem > len) nelem--;

  size_t n = nelem;

  if (to_idx > from_idx) {
    char *sp = str + from_idx + nelem;
    char *dsp = str + to_idx + nelem;

    while (nelem--) *--dsp = *--sp;
    return (n - nelem) - 1;
  }

  while (from_idx + nelem > len) nelem--;
  n = nelem;

  char *sp = str + from_idx;
  char *dsp = str + to_idx;

  while (nelem) {
    ifnot (*sp) {  // stop at the first null byte
      *dsp = '\0'; // this differs in memmove()
      break;
    }

    *dsp++ = *sp++;
    nelem--;
  }

  return n - nelem;
}

static size_t cstring_cp (char *dest, size_t dest_len, const char *src, size_t nelem) {
  size_t num = (nelem > (dest_len - 1) ? dest_len - 1 : nelem);
  size_t len = (NULL is src ? 0 : cstring_byte_cp (dest, src, num));
  dest[len] = '\0';
  return len;
}

static size_t cstring_cp_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return cstring_cp (dest, dest_len, bytes, len);
}

/* the signature changed as in this namespace, size has been already computed */
static char *cstring_dup (const char *src, size_t len) {
  /* avoid recomputation */
  // size_t len = bytelen (src);
  char *dest = Alloc (len + 1);
  cstring_cp (dest, len + 1, src, len);
  return dest;
}

static char *cstring_trim_end (char *bytes, char c) {
  char *sp = cstring_byte_null_in_str (bytes);
  sp--;

  while (sp >= bytes) {
    if (*sp isnot c) break;
    *sp = '\0';
    if (sp is bytes) break;
    sp--;
  }
  return bytes;
}

static char *cstring_substr (char *dest, size_t len, char *src, size_t src_len, size_t idx) {
  if (src_len < idx + len) {
    dest[0] = '\0';
    return NULL;
  }

  for (size_t i = 0; i < len; i++) dest[i] = src[i+idx];
  dest[len] = '\0';
  return dest;
}

/* This is itoa version 0.4, written by Lukás Chmela and released under GPLv3,
 * Original Source:  http://www.strudel.org.uk/itoa/
 */

/* this is for integers (i wonder maybe is better to use *printf() - but i hate to
   do it, because of the short code and its focus to this specific functionality) */
static char *cstring_itoa (int value, char *result, int base) {
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
        [35 + (tmp_value - (value * base))];
  } while (value);

  /* Apply negative sign */
  if (0 > tmp_value) *ptr++ = '-';

  *ptr-- = '\0';

  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
   }

  return result;
}

static char *cstring_extract_word_at (
const char *bytes,  size_t bsize, char *word, size_t wsize, const char *Nwtype, size_t Nwsize,
                                               int cur_idx, int *fidx, int *lidx) {
  if (NULL is bytes or 0 is bsize or (int) bsize <= cur_idx) {
    *lidx = cur_idx;
    return NULL;
  }

  if (IS_SPACE (bytes[cur_idx]) or
      IS_CNTRL (bytes[cur_idx]) or
      NULL isnot memchr (Nwtype, bytes[cur_idx], Nwsize)) {
    *lidx = cur_idx;
    return NULL;
  }

  while (cur_idx > 0 and
      IS_SPACE (bytes[cur_idx]) is 0 and
      IS_CNTRL (bytes[cur_idx]) is 0 and
      NULL is memchr (Nwtype, bytes[cur_idx], Nwsize))
    cur_idx--;

  if (cur_idx isnot 0 or (
      IS_SPACE (bytes[cur_idx]) or
      IS_CNTRL (bytes[cur_idx]) or
      NULL isnot memchr (Nwtype, bytes[cur_idx], Nwsize)))
    cur_idx++;

  *fidx = cur_idx;

  int widx = 0;
  while (cur_idx < (int) bsize and
      IS_SPACE (bytes[cur_idx]) is 0 and
      IS_CNTRL (bytes[cur_idx]) is 0 and
      NULL is memchr (Nwtype, bytes[cur_idx], Nwsize) and
      widx <= (int) wsize)
    word[widx++] = bytes[cur_idx++];

  *lidx = cur_idx - 1;

  word[widx] = '\0';

  return word;
}

static void cstring_tok_release (cstring_tok *this) {
  if (NULL is this) return;
  for (int i = 0; i < this->num_tokens; i++)
    free (this->tokens[i]);

  free (this->tokens);
  free (this->length);
  free (this);
  this = NULL;
}

static cstring_tok *cstring_tokenize (cstring_tok *this,
 const char *buf, const char *tok, CstringTok_cb cb, void *obj) {
  if (NULL is tok or *tok is '\0') return this;

  size_t toklen = bytelen (tok);

  cstring_tok *ts = this;

  int ts_isnull = (NULL is ts);
  if (ts_isnull) {
    ts = Alloc (sizeof (cstring_tok));
    ts->num_tokens = 0;
    ts->tokens = Alloc (sizeof (char *));
    ts->length = NULL;
  }

  char *sp = (char *) buf;
  char *p = sp;

  int end = 0;
  for (;;) {
    if (end) break;
    ifnot (*sp) {
      end = 1;
      goto tokenize;
    }

    if (cstring_eq_n (sp, tok, toklen)) {
tokenize:;
      size_t len = sp - p;
      /* ifnot (len) {
         sp++; p = sp;
         continue;
      } when commented, this broke once the code */

      char s[len + 1];
      cstring_cp (s, len + 1, p, len);

      ifnot (NULL is cb) {
        int retval;
        if (CSTRING_TOK_NOTOK is (retval = cb (ts, s, len, obj))) {
          if (ts_isnull)
            cstring_tok_release (ts);
          return NULL;
        }

        if (retval is CSTRING_TOK_RETURN)
          return ts;
      } else {
        ts->num_tokens++;
        ts->tokens = Realloc (ts->tokens, sizeof (char *) * ts->num_tokens);
        ts->tokens[ts->num_tokens - 1] = Alloc (len + 1);
        cstring_cp (ts->tokens[ts->num_tokens - 1], len + 1, s, len);
        ts->length = (1 is ts->num_tokens
            ? Alloc (sizeof (int))
            : Realloc (ts->length, sizeof (int) * ts->num_tokens));
        ts->length[ts->num_tokens - 1] = len;
      }

      sp++;
      p = sp;
      continue;
    }

    sp++;
  }

  return ts;
}

public cstring_T __init_cstring__ (void) {
  return (cstring_T) {
    .self = (cstring_self) {
      .cp = cstring_cp,
      .eq = cstring_eq,
      .cat = cstring_cat,
      .dup = cstring_dup,
      .eq_n = cstring_eq_n,
      .itoa = cstring_itoa,
      .cmp_n = cstring_cmp_n,
      .substr = cstring_substr,
      .cp_fmt = cstring_cp_fmt,
      .tokenize = cstring_tokenize,
      .tok_release = cstring_tok_release,
      .bytes_in_str = strstr,
      .extract_word_at = cstring_extract_word_at,
      .trim = (cstring_trim_self) {
        .end = cstring_trim_end,
      },
      .byte = (cstring_byte_self) {
        .cp = cstring_byte_cp,
        .mv = cstring_byte_mv,
        .cp_all = cstring_byte_cp_all,
        .in_str = cstring_byte_in_str,
        .in_str_r = cstring_byte_in_str_r,
        .null_in_str = cstring_byte_null_in_str
      },
    }
  };
}
