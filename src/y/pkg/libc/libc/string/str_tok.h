typedef struct {
  int num_tokens;
  int num_alloc_tokens;
  char **tokens;
} StrTokenType;

typedef int (*StrTokenCb) (StrTokenType *, char *, void *);

