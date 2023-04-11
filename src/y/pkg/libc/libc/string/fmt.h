typedef struct fmtType fmtType;

typedef void (*fmtOutputChar) (fmtType *, int);

struct fmtType {
  size_t counter;
  size_t ps_size;
  char *ps;
  const char *pf;

#define WIDTH_UNSET          -1
  int width;
#define PRECISION_UNSET      -1
  int precision;

#define ALIGN_UNSET           0
#define ALIGN_RIGHT           1
#define ALIGN_LEFT            2
  unsigned int align:2;
  unsigned int is_square:1;
  unsigned int is_space:1;
  unsigned int is_dot:1;
  unsigned int is_star_w:1;
  unsigned int is_star_p:1;

#define INT_LEN_DEFAULT       0
#define INT_LEN_LONG          1
#define INT_LEN_LONG_LONG     2
#define INT_LEN_SHORT         3
#define INT_LEN_CHAR          4

  unsigned int a_long:3;
  unsigned int rfu:6;

  char pad;

  fmtOutputChar output_char;
  void *user_data;

  char slop[5];
};
