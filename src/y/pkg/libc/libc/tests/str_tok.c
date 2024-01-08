/* num-tests: 3 */
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

  ifnot (2 == tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting two items found %d\n", tokens->num_tokens);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[0], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[0]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[1], "gkl")) {
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

  ifnot (4 == tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 4 items found %d\n", tokens->num_tokens);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[0], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[0]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[1], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[1]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[2], "gkl")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'gkl' found %s\n", tokens->tokens[2]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[3], "")) {
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

  ifnot (6 == tokens->num_tokens) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 6 items found %d\n", tokens->num_tokens);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[0], "")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting empty string found %s\n", tokens->tokens[0]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[1], "αaδαb")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'αaδab' found %s\n", tokens->tokens[1]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[2], "asdf")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'asdf' found %s\n", tokens->tokens[2]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[3], "gkl")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'gkl' found %s\n", tokens->tokens[3]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[4], "βbγαc")) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'βbγαc' found %s\n", tokens->tokens[4]);
    goto theend;
  }

  ifnot (str_eq (tokens->tokens[5], "")) {
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
  int num_tests = 0;
  int failed = 0;

  num_tests++;  total++;
  if (first_test (total) == -1) failed++;
  num_tests++;  total++;
  if (second_test (total) == -1) failed++;
  num_tests++;  total++;
  if (third_test (total) == -1) failed++;

  return failed;
}
