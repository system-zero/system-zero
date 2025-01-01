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
