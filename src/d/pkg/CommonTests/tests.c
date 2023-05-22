#define MAX_LEN_MSG 128

int failedTests = 0;
int okTests = 0;
int numTests = 0;
char msgStr[MAX_LEN_MSG];
int msgBytesWritten; // for now used only on errors
decimal_t decString;

#define INTRO(__m__, __mlen__) do {                  \
  numTests++;                                        \
  decString.base = 10;                               \
  char *nstr = int_to_string (&decString, numTests); \
  size_t _len = decString.size + 3 + __mlen__ + 1;   \
  char buf[_len + 1];                                \
  snprintf (buf, _len + 1, "[%s] %s", nstr, __m__);  \
  sys_write (1, buf, _len);                          \
} while (0)


#define N_FUNC_TEST_TO_STR(__n__) ({                 \
  __n__++;                                           \
  size_t num = bytelen (__func__);                   \
  str_copy (msgStr, MAX_LEN_MSG, __func__, num);     \
  msgStr[num++] = ' ';                               \
  decString.base = 10;                               \
  char *nstr = int_to_string (&decString, __n__);    \
  str_copy (msgStr + num, MAX_LEN_MSG - num, nstr, decString.size); \
  num += decString.size;                             \
  num;                                               \
})

#define N_TEST_TO_STR(__f__, __n__) ({               \
  __n__++;                                           \
  size_t num = bytelen (__f__);                      \
  str_copy (msgStr, MAX_LEN_MSG, __f__, num);        \
  msgStr[num++] = ' ';                               \
  decString.base = 10;                               \
  char *nstr = int_to_string (&decString, __n__);    \
  str_copy (msgStr + num, MAX_LEN_MSG - num, nstr, decString.size); \
  num += decString.size;                             \
  num;                                               \
})

#define MSGSTRERR(__s__) ({                          \
  size_t l_ = bytelen (__s__);                       \
  l_ = str_copy (msgStr, MAX_LEN_MSG, __s__, l_);    \
  msgBytesWritten = l_;                              \
})

#define SUMMARY() do {                               \
  decString.base = 10;                               \
  char *nstr = int_to_string (&decString, numTests); \
  size_t _len = 1024;                                \
  char buf[_len];                                    \
  int wr = snprintf (buf, _len, "NumTests : [%s]\n", nstr);\
  nstr = int_to_string (&decString, failedTests);    \
  wr += snprintf (buf + wr, _len - wr, "  Failed : [%s]\n", nstr);\
  nstr = int_to_string (&decString, okTests);        \
  wr += snprintf (buf + wr, _len - wr, "  Passed : [%s]\n", nstr);\
  sys_write (1, buf, wr);                            \
} while (0)

// open the error database with fopen 11 04 22 21 59 11

enum {
  TEST_SUCCESS = 0,
  FOPEN_ERROR
};

static void exit_hard (const char *msg, int v) {
  MSGSTRERR(msg);
  sys_write (2, msgStr, msgBytesWritten);
  exit (v);
}

static void test_str_eq (const char *a, const char *b, const char *msg, size_t len) {
  INTRO(msg, len);

  ifnot (str_eq (a, b)) {
    sys_write (2, ": failed\n", 9);
    failedTests++;
    return;
  }

  sys_write (1, ": ok\n", 5);
  okTests++;
}

static void test_int_eq (int a, int b, const char *msg, size_t len) {
  INTRO(msg, len);

  ifnot (a is b) {
    sys_write (2, ": failed\n", 9);
    failedTests++;
    return;
  }

  sys_write (1, ": ok\n", 5);
  okTests++;
}
