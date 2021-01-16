#define REQUIRE_STDIO

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_RE_TYPE       DONOT_DECLARE

#include <z/cenv.h>

#define MAX_BRANCHES 100
#define MAX_BRACKETS 100

struct re_cap {
  const char *ptr;
  int len;
};

struct bracket_pair {
  const char *ptr;  /* Points to the first char after '(' in regex  */
  int len;          /* Length of the text between '(' and ')'       */
  int branches;     /* Index in the branches array for this pair    */
  int num_branches; /* Number of '|' in this bracket pair           */
};

struct branch {
  int bracket_index;    /* index for 'struct bracket_pair brackets' */
                        /* array defined below                      */
  const char *schlong;  /* points to the '|' character in the regex */
};

struct regex_info {
  /*
   * Describes all bracket pairs in the regular expression.
   * First entry is always present, and grabs the whole regex.
   */
  struct bracket_pair brackets[MAX_BRACKETS];
  int num_brackets;

  /*
   * Describes alternations ('|' operators) in the regular expression.
   * Each branch falls into a specific branch pair.
   */
  struct branch branches[MAX_BRANCHES];
  int num_branches;

  /* Array of captures provided by the user */
  struct re_cap *caps;
  int num_caps;

  /* E.g. RE_IGNORE_CASE */
  int flags;

  /* EXTENSION: 
   * the start byte index that occured the match */
  int match_idx;

  /* EXTENSION: 
   * byte length of the full match */
  int match_len;

  /* EXTENSION: 
   * total final captured substrings */
  int total_caps;
};

static int is_digit (int c) {
  return (c >= '0' and c <= '9');
}

static int is_space (int c) {
  return (c is ' ') or (c is '\t');
}

static int is_metacharacter (const unsigned char *s) {
  static const char *metacharacters = "^$().[]*+?|\\Ssdbfnrtv";
  return Cstring.byte.in_str (metacharacters, *s) != NULL;
}

static int is_hexchar (int c) {
  return (c >= '0' and c <= '9') or Cstring.byte.in_str ("abcdefABCDEF", c);
}

static int is_quantifier (const char *re) {
  return re[0] == '*' || re[0] == '+' || re[0] == '?';
}

static void re_reset_captures (re_t *re) {
  re->match_len = re->match_idx = 0;
  re->match_ptr = NULL;

  ifnot (re->flags & RE_PATTERN_IS_STRING_LITERAL) {
    String.release (re->match);
    re->match = NULL;
  }

  if (re->cap is NULL) return;
  for (int i = 0; i < re->num_caps; i++) {
    if (NULL is re->cap[i]) continue;
    free (re->cap[i]);
    re->cap[i] = NULL;
  }
}

static void re_release_captures (re_t *re) {
  re_reset_captures (re);
  if (re->cap is NULL) return;
  free (re->cap);
  re->cap = NULL;
}

static void re_allocate_captures (re_t *re, int num) {
  re->num_caps = (0 > num ? 0 : num);
  re->cap = Alloc (sizeof (capture_t) * re->num_caps);
}

static void re_release_pat (re_t *re) {
  if (NULL is re->pat) return;
  String.release (re->pat);
  re->pat = NULL;
}

static void re_release (re_t *re) {
  if (re is NULL) return;
  re_release_pat (re);
  re_release_captures (re);
  free (re);
}

static re_t *re_new (char *pat, int flags, int num_caps, ReCompile_cb compile) {
  re_t *re = Alloc (sizeof (re_t));
  re->flags |= flags;
  re->pat = String.new_with (pat);
  compile (re);
  re_allocate_captures (re, num_caps);
  re->match = NULL;
  return re;
}

/* slre:
   https://github.com/cesanta/slre
 */

/*
 * Copyright (c) 2004-2013 Sergey Lyubka <valenok@gmail.com>
 * Copyright (c) 2013 Cesanta Software Limited
 * All rights reserved
 *
 * This library is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this library under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, as set out in <http://cesanta.com/i_products.html>.
 */

#define FAIL_IF(condition, error_code) if (condition) return (error_code)

#define ARRAY_SIZE ARRLEN

#ifdef RE_DEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

static int re_op_len (const char *re) {
  return re[0] == '\\' && re[1] == 'x' ? 4 : re[0] == '\\' ? 2 : 1;
}

static int re_set_len (const char *re, int re_len) {
  int len = 0;

  while (len < re_len && re[len] != ']') {
    len += re_op_len (re + len);
  }

  return len <= re_len ? len + 1 : -1;
}

static int re_get_op_len (const char *re, int re_len) {
  return re[0] == '[' ? re_set_len (re + 1, re_len - 1) + 1 : re_op_len (re);
}

static int re_toi (int x) {
  return is_digit (x) ? x - '0' : x - 'W';
}

static int re_hextoi (const unsigned char *s) {
  return (re_toi (Ustring.to_lower (s[0])) << 4) | re_toi (Ustring.to_lower (s[1]));
}

static int re_match_op (const unsigned char *re, const unsigned char *s,
                    struct regex_info *info) {
  int result = 0;
  switch (*re) {
    case '\\':
      /* Metacharacters */
      switch (re[1]) {
        case 'S': FAIL_IF(is_space (*s), RE_NO_MATCH); result++; break;
        case 's': FAIL_IF(!is_space (*s), RE_NO_MATCH); result++; break;
        case 'd': FAIL_IF(!is_digit (*s), RE_NO_MATCH); result++; break;
        case 'b': FAIL_IF(*s != '\b', RE_NO_MATCH); result++; break;
        case 'f': FAIL_IF(*s != '\f', RE_NO_MATCH); result++; break;
        case 'n': FAIL_IF(*s != '\n', RE_NO_MATCH); result++; break;
        case 'r': FAIL_IF(*s != '\r', RE_NO_MATCH); result++; break;
        case 't': FAIL_IF(*s != '\t', RE_NO_MATCH); result++; break;
        case 'v': FAIL_IF(*s != '\v', RE_NO_MATCH); result++; break;

        case 'x':
          /* Match byte, \xHH where HH is hexadecimal byte representaion */
          FAIL_IF(re_hextoi (re + 2) != *s, RE_NO_MATCH);
          result++;
          break;

        default:
          /* Valid metacharacter check is done in bar() */
          FAIL_IF(re[1] != s[0], RE_NO_MATCH);
          result++;
          break;
      }
      break;

    case '|': FAIL_IF(1, RE_INTERNAL_ERROR); break;
    case '$': FAIL_IF(1, RE_NO_MATCH); break;
    case '.': result++; break;

    default:
      if (info->flags & RE_IGNORE_CASE) {
        FAIL_IF(Ustring.to_lower (*re) != Ustring.to_lower (*s), RE_NO_MATCH);
      } else {
        FAIL_IF(*re != *s, RE_NO_MATCH);
      }
      result++;
      break;
  }

  return result;
}

static int re_match_set (const char *re, int re_len, const char *s,
                     struct regex_info *info) {
  int len = 0, result = -1, invert = re[0] == '^';

  if (invert) re++, re_len--;

  while (len <= re_len && re[len] != ']' && result <= 0) {
    /* Support character range */
    if (re[len] != '-' && re[len + 1] == '-' && re[len + 2] != ']' &&
        re[len + 2] != '\0') {
      result = info->flags &  RE_IGNORE_CASE ?
        Ustring.to_lower (*s) >= Ustring.to_lower (re[len]) && Ustring.to_lower (*s) <= Ustring.to_lower (re[len + 2]) :
        *s >= re[len] && *s <= re[len + 2];
      len += 3;
    } else {
      result = re_match_op ((const unsigned char *) re + len, (const unsigned char *) s, info);
      len += re_op_len (re + len);
    }
  }
  return (!invert && result > 0) || (invert && result <= 0) ? 1 : -1;
}

static int re_doh (const char *s, int s_len, struct regex_info *info, int bi);

static int re_bar (const char *re, int re_len, const char *s, int s_len,
               struct regex_info *info, int bi) {
  /* i is offset in re, j is offset in s, bi is brackets index */
  int i, j, n, step;

  for (i = j = 0; i < re_len && j <= s_len; i += step) {

    /* Handle quantifiers. Get the length of the chunk. */
    step = re[i] == '(' ? info->brackets[bi + 1].len + 2 :
      re_get_op_len (re + i, re_len - i);

    DBG(("%s [%.*s] [%.*s] re_len=%d step=%d i=%d j=%d\n", __func__,
         re_len - i, re + i, s_len - j, s + j, re_len, step, i, j));

    FAIL_IF(is_quantifier (&re[i]), RE_UNEXPECTED_QUANTIFIER_ERROR);
    FAIL_IF(step <= 0, RE_INVALID_CHARACTER_SET_ERROR);

    if (i + step < re_len && is_quantifier (re + i + step)) {
      DBG(("QUANTIFIER: [%.*s]%c [%.*s]\n", step, re + i,
           re[i + step], s_len - j, s + j));
      if (re[i + step] == '?') {
        int result = re_bar (re + i, step, s + j, s_len - j, info, bi);
        j += result > 0 ? result : 0;
        i++;
      } else if (re[i + step] == '+' || re[i + step] == '*') {
        int j2 = j, nj = j, n1, n2 = -1, ni, non_greedy = 0;

        /* Points to the regexp code after the quantifier */
        ni = i + step + 1;
        if (ni < re_len && re[ni] == '?') {
          non_greedy = 1;
          ni++;
        }

        do {
          if ((n1 = re_bar (re + i, step, s + j2, s_len - j2, info, bi)) > 0) {
            j2 += n1;
          }
          if (re[i + step] == '+' && n1 < 0) break;

          if (ni >= re_len) {
            /* After quantifier, there is nothing */
            nj = j2;
          } else if ((n2 = re_bar (re + ni, re_len - ni, s + j2,
                               s_len - j2, info, bi)) >= 0) {
            /* Regex after quantifier matched */
            nj = j2 + n2;
          }
          if (nj > j && non_greedy) break;
        } while (n1 > 0);

        /*
         * Even if we found one or more pattern, this branch will be executed,
         * changing the next captures.
         */
        if (n1 < 0 && n2 < 0 && re[i + step] == '*' &&
            (n2 = re_bar (re + ni, re_len - ni, s + j, s_len - j, info, bi)) > 0) {
          nj = j + n2;
        }

        DBG(("STAR/PLUS END: %d %d %d %d %d\n", j, nj, re_len - ni, n1, n2));
        FAIL_IF(re[i + step] == '+' && nj == j, RE_NO_MATCH);

        /* If while loop body above was not executed for the * quantifier,  */
        /* make sure the rest of the regex matches                          */
        FAIL_IF(nj == j && ni < re_len && n2 < 0, RE_NO_MATCH);

        /* Returning here cause we've matched the rest of RE already */
        return nj;
      }
      continue;
    }

    if (re[i] == '[') {
      n = re_match_set (re + i + 1, re_len - (i + 2), s + j, info);
      DBG(("SET %.*s [%.*s] -> %d\n", step, re + i, s_len - j, s + j, n));
      FAIL_IF(n <= 0, RE_NO_MATCH);
      j += n;
    } else if (re[i] == '(') {
      n = RE_NO_MATCH;
      bi++;
      FAIL_IF(bi >= info->num_brackets, RE_INTERNAL_ERROR);
      DBG(("CAPTURING [%.*s] [%.*s] [%s]\n",
           step, re + i, s_len - j, s + j, re + i + step));

      if (re_len - (i + step) <= 0) {
        /* Nothing follows brackets */
        n = re_doh (s + j, s_len - j, info, bi);
      } else {
        int j2;
        for (j2 = 0; j2 <= s_len - j; j2++) {
          if ((n = re_doh (s + j, s_len - (j + j2), info, bi)) >= 0 &&
              re_bar (re + i + step, re_len - (i + step),
                  s + j + n, s_len - (j + n), info, bi) >= 0) break;
        }
      }

      DBG(("CAPTURED [%.*s] [%.*s]:%d\n", step, re + i, s_len - j, s + j, n));
      FAIL_IF(n < 0, n);

      if (info->caps != NULL && n > 0) {
        info->caps[bi - 1].ptr = s + j;
        info->caps[bi - 1].len = n;
//        info->total_caps++;
        info->total_caps = bi;
      }
      j += n;
    } else if (re[i] == '^') {
      FAIL_IF(j != 0, RE_NO_MATCH);
    } else if (re[i] == '$') {
      FAIL_IF(j != s_len, RE_NO_MATCH);
    } else {
      FAIL_IF(j >= s_len, RE_NO_MATCH);
      n = re_match_op ((const unsigned char *) (re + i), (const unsigned char *) (s + j), info);
      FAIL_IF(n <= 0, n);
      j += n;
    }
  }

  return j;
}

/* Process branch points */
static int re_doh (const char *s, int s_len, struct regex_info *info, int bi) {
  const struct bracket_pair *b = &info->brackets[bi];
  int i = 0, len, result;
  const char *p;

  do {
    p = i == 0 ? b->ptr : info->branches[b->branches + i - 1].schlong + 1;
    len = b->num_branches == 0 ? b->len :
      i == b->num_branches ? (int) (b->ptr + b->len - p) :
      (int) (info->branches[b->branches + i].schlong - p);
    DBG(("%s %d %d [%.*s] [%.*s]\n", __func__, bi, i, len, p, s_len, s));
    result = re_bar (p, len, s, s_len, info, bi);
    DBG(("%s <- %d\n", __func__, result));
  } while (result <= 0 && i++ < b->num_branches);  /* At least 1 iteration */

  return result;
}

static int re_baz (const char *s, int s_len, struct regex_info *info) {
  int i, result = -1, is_anchored = info->brackets[0].ptr[0] == '^';

  for (i = 0; i <= s_len; i++) {
    result = re_doh (s + i, s_len - i, info, 0);

    if (result >= 0) {
      /* EXTENSION */
      info->match_idx = i;
      info->match_len = result;
      /**/
      result += i;
      break;
    }
    if (is_anchored) break;
  }

  return result;
}

static void re_setup_branch_points (struct regex_info *info) {
  int i, j;
  struct branch tmp;

  /* First, sort branches. Must be stable, no qsort. Use bubble algo. */
  for (i = 0; i < info->num_branches; i++) {
    for (j = i + 1; j < info->num_branches; j++) {
      if (info->branches[i].bracket_index > info->branches[j].bracket_index) {
        tmp = info->branches[i];
        info->branches[i] = info->branches[j];
        info->branches[j] = tmp;
      }
    }
  }

  /*
   * For each bracket, set their branch points. This way, for every bracket
   * (i.e. every chunk of regex) we know all branch points before matching.
   */
  for (i = j = 0; i < info->num_brackets; i++) {
    info->brackets[i].num_branches = 0;
    info->brackets[i].branches = j;
    while (j < info->num_branches && info->branches[j].bracket_index == i) {
      info->brackets[i].num_branches++;
      j++;
    }
  }
}

static int re_foo (const char *re, int re_len, const char *s, int s_len,
               struct regex_info *info) {
  int i, step, depth = 0;

  /* First bracket captures everything */
  info->brackets[0].ptr = re;
  info->brackets[0].len = re_len;
  info->num_brackets = 1;

  /* Make a single pass over regex string, memorize brackets and branches */
  for (i = 0; i < re_len; i += step) {
    step = re_get_op_len (re + i, re_len - i);

    if (re[i] == '|') {
      FAIL_IF(info->num_branches >= (int) ARRAY_SIZE(info->branches),
              RE_TOO_MANY_BRANCHES_ERROR);
      info->branches[info->num_branches].bracket_index =
        info->brackets[info->num_brackets - 1].len == -1 ?
        info->num_brackets - 1 : depth;
      info->branches[info->num_branches].schlong = &re[i];
      info->num_branches++;
    } else if (re[i] == '\\') {
      FAIL_IF(i >= re_len - 1, RE_INVALID_METACHARACTER_ERROR);
      if (re[i + 1] == 'x') {
        /* Hex digit specification must follow */
        FAIL_IF(re[i + 1] == 'x' && i >= re_len - 3, RE_INVALID_METACHARACTER_ERROR);
        FAIL_IF(re[i + 1] ==  'x' && !(is_hexchar (re[i + 2]) &&
                is_hexchar (re[i + 3])), RE_INVALID_METACHARACTER_ERROR);
      } else {
        FAIL_IF(!is_metacharacter ((const unsigned char *) re + i + 1),
                RE_INVALID_METACHARACTER_ERROR);
      }
    } else if (re[i] == '(') {
      FAIL_IF(info->num_brackets >= (int) ARRAY_SIZE(info->brackets),
              RE_TOO_MANY_BRACKETS_ERROR);
      depth++;  /* Order is important here. Depth increments first. */
      info->brackets[info->num_brackets].ptr = re + i + 1;
      info->brackets[info->num_brackets].len = -1;
      info->num_brackets++;
      FAIL_IF(info->num_caps > 0 && info->num_brackets - 1 > info->num_caps,
              RE_CAPS_ARRAY_TOO_SMALL_ERROR);
    } else if (re[i] == ')') {
      int ind = info->brackets[info->num_brackets - 1].len == -1 ?
        info->num_brackets - 1 : depth;
      info->brackets[ind].len = (int) (&re[i] - info->brackets[ind].ptr);
      DBG(("SETTING BRACKET %d [%.*s]\n",
           ind, info->brackets[ind].len, info->brackets[ind].ptr));
      depth--;
      FAIL_IF(depth < 0, RE_UNBALANCED_BRACKETS_ERROR);
      FAIL_IF(i > 0 && re[i - 1] == '(', RE_NO_MATCH);
    }
  }

  FAIL_IF(depth != 0, RE_UNBALANCED_BRACKETS_ERROR);
  re_setup_branch_points(info);

  return re_baz (s, s_len, info);
}

/* this is like slre_match(), with an aditional argument and three extra fields
 * in the slre regex_info structure */
static int re_match (re_t *re, const char *regexp, const char *s,
             int s_len, struct re_cap *caps, int num_caps, int flags) {
  struct regex_info info;

  info.flags = flags;
  info.num_brackets = info.num_branches = 0;
  info.num_caps = num_caps;
  info.caps = caps;

  info.match_idx = info.match_len = -1;
  info.total_caps = 0;

  int retval = re_foo (regexp, (int) bytelen(regexp), s, s_len, &info);

  if (0 <= retval) {
    re->match_idx = info.match_idx;
    re->match_len = info.match_len;
    re->total_caps = info.total_caps;
    re->match_ptr = (char *) s + info.match_idx;
  }

  return retval;
}

static int re_compile (re_t *re) {
  ifnot (Cstring.cmp_n (re->pat->bytes, "(?i)", 4)) {
    re->flags |= RE_IGNORE_CASE;
    String.delete_numbytes_at (re->pat, 4, 0);
  }

  return OK;
}

static int re_exec (re_t *re, char *buf, size_t buf_len) {
  re->retval = RE_NO_MATCH;
  if (re->pat->num_bytes is 1 and
     (re->pat->bytes[0] is '^' or
      re->pat->bytes[0] is '$' or
      re->pat->bytes[0] is '|'))
    return re->retval;

  do {
    struct re_cap cap[re->num_caps];
    for (int i = 0; i < re->num_caps; i++) cap[i].len = 0;
    re->retval = re_match (re, re->pat->bytes, buf, buf_len,
        cap, re->num_caps, re->flags);

    if (re->retval is RE_CAPS_ARRAY_TOO_SMALL_ERROR) {
      re_release_captures (re);
      re_allocate_captures (re, re->num_caps + (re->num_caps / 2));

      continue;
    }

    if (0 > re->retval) goto theend;
    re->match = String.new_with (re->match_ptr);
    String.clear_at (re->match, re->match_len);

    for (int i = 0; i < re->total_caps; i++) {
      re->cap[i] = Alloc (sizeof (capture_t));
      re->cap[i]->ptr = cap[i].ptr;
      re->cap[i]->len = cap[i].len;
    }
  } while (0);

theend:
  return re->retval;
}

static string_t *re_parse_substitute (re_t *re, char *sub, char *replace_buf) {
  string_t *substr = String.new (64);

  char *sub_p = sub;
  while (*sub_p) {
    switch (*sub_p) {
      case '\\':
        if (*(sub_p + 1) is 0) {
          Cstring.cp (re->errmsg, RE_MAXLEN_ERR_MSG, "awaiting escaped char, found (null byte) 0", RE_MAXLEN_ERR_MSG - 1);
          goto theerror;
        }

        switch (*++sub_p) {
          case '&':
            String.append_byte (substr, '&');
            sub_p++;
            continue;

          case 's':
            String.append_byte (substr, ' ');
            sub_p++;
            continue;

          case '\\':
            String.append_byte (substr, '\\');
            sub_p++;
            continue;

          case '1'...'9':
            {
              int idx = 0;
              while (*sub_p and ('0' <= *sub_p and *sub_p <= '9')) {
                idx = (10 * idx) + (*sub_p - '0');
                sub_p++;
              }
              idx--;

              if (0 > idx or idx + 1 > re->total_caps) goto theerror;

              char buf[re->cap[idx]->len + 1];
              Cstring.cp (buf, re->cap[idx]->len + 1, re->cap[idx]->ptr, re->cap[idx]->len);
              String.append_with (substr, buf);
            }

            continue;

          default:
            snprintf (re->errmsg, 256, "awaiting \\,&,s[0..9,...], got %d [%c]",
                *sub_p, *sub_p);
            goto theerror;
        }

      case '&':
        String.append_with (substr, replace_buf);
        break;

      default:
        String.append_byte (substr, *sub_p);
     }

    sub_p++;
  }

  return substr;

theerror:
  String.release (substr);
  return NULL;
}

public re_T __init_re__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (ustring);

  return (re_T) {
    .self = (re_self) {
      .exec = re_exec,
      .new = re_new,
      .release = re_release,
      .release_captures = re_release_captures,
      .allocate_captures = re_allocate_captures,
      .reset_captures = re_reset_captures,
      .release_pat = re_release_pat,
      .parse_substitute = re_parse_substitute,
      .compile = re_compile,
    }
  };
}
