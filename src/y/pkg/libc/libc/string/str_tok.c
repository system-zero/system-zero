// provides: StrTokenType *str_tok (const char *, utf8, StrTokenType *, StrTokenCb, void *)
// provides: void str_tok_release (StrTokenType **)
// requires: string/str_new.c
// requires: string/str_copy.c
// requires: utf8/utf8_code.c
// requires: stdlib/alloc.c
// requires: string/str_tok.h

StrTokenType *str_tok (const char *buf, utf8 tok, StrTokenType *obj, StrTokenCb cb, void *userdata) {
  if (NULL == buf)
    return obj;

  if (NULL == obj) {
    obj = Alloc (sizeof (StrTokenType));
    obj->num_tokens = 0;
    obj->num_alloc_tokens = 4;
    obj->tokens = Alloc (sizeof (char *) * obj->num_alloc_tokens);
  }

  char *sp = (char *) buf;
  char *p = sp;

  int ulen = 0;
  int end = 0;

  for (;;) {
    if (end)
      break;

    if ('\0' == *sp) {
      end = 1;
      goto tokenize;
    }

    ulen = 0;
    utf8 code = utf8_code_and_len (sp, &ulen);

    if (code == tok) {
      tokenize:;

      size_t toklen = sp - p;

      if (NULL != cb) {
        char s[toklen + 1];
        str_copy (s, toklen + 1, p, toklen);
        if (0 != cb (obj, s, userdata))
          return obj;
      } else {
        char *token = str_new_with_len (p, toklen);
        obj->num_tokens++;
        if (obj->num_tokens == obj->num_alloc_tokens) {
          obj->num_alloc_tokens *= 2;
          obj->tokens = Realloc (obj->tokens, sizeof (char *) * obj->num_alloc_tokens);
        }

        obj->tokens[obj->num_tokens - 1] = token;
      }

      sp += ulen;
      p = sp;
      continue;
    }

    sp++;
  }

  return obj;
}

void str_tok_release (StrTokenType **objp) {
  if (NULL == *objp)
    return;

  for (int i = 0; i < (*objp)->num_tokens; i++)
    Release ((*objp)->tokens[i]);

  Release ((*objp)->tokens);
  Release (*objp);
  *objp = NULL;
}

/* test {
// num-tests: 3

#define REQUIRE_STR_TOK
#define REQUIRE_STR_EQ
#define REQUIRE_ATOI
#define REQUIRE_STDIO

#include <libc.h>

static int first_test (int total) {
  int retval = -1;

  tostdout ("[%d] testing str_tok() %s - ", total, __func__);

  char buf[] = "asdf|gkl";

  StrTokenType *tokens = str_tok (buf, '|', NULL, NULL, NULL);

  if (tokens == NULL) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (2 != tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting two items found %d\n", tokens->num_tokens);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[0], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[0]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[1], "gkl")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'gkl' found %s\n", tokens->tokens[1]);
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  str_tok_release (&tokens);
  return retval;
}

static int second_test (int total) {
  int retval = -1;

  tostdout ("[%d] testing str_tok() %s - ", total, __func__);

  char buf[] = "|asdf|gkl|";

  StrTokenType *tokens = str_tok (buf, '|', NULL, NULL, NULL);

  if (tokens == NULL) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (4 != tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 4 items found %d\n", tokens->num_tokens);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[0], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[0]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[1], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[1]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[2], "gkl")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'gkl' found %s\n", tokens->tokens[2]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[3], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[3]);
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  str_tok_release (&tokens);
  return retval;
}

static int third_test (int total) {
  int retval = -1;

  tostdout ("[%d] testing str_tok() %s - ", total, __func__);

  char buf[] = "|αaδαb|asdf|gkl|βbγαc|";

  StrTokenType *tokens = str_tok (buf, '|', NULL, NULL, NULL);

  if (tokens == NULL) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (6 != tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 6 items found %d\n", tokens->num_tokens);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[0], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[0]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[1], "αaδαb")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'αaδab' found %s\n", tokens->tokens[1]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[2], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[2]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[3], "gkl")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'gkl' found %s\n", tokens->tokens[3]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[4], "βbγαc")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'βbγαc' found %s\n", tokens->tokens[4]);
    goto theend;
  }

  if (0 == str_eq (tokens->tokens[5], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[5]);
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  str_tok_release (&tokens);
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;
  total++;
  if (second_test (total) == -1) failed++;
  total++;
  if (third_test (total) == -1) failed++;

  return failed;
}
} */
