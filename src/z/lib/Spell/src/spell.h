#ifndef SPELL_HDR
#define SPELL_HDR

#define SPELL_CHANGED_WORD 1

#ifndef Notword
#define Notword ".,?/+*-=~%<>[](){}\\'\";"
#endif

#ifndef Notword_len
#define Notword_len 22
#endif

#define SPELL_NOTWORD Notword "012345678#:`$_"
#define SPELL_NOTWORD_LEN (Notword_len + 14)

#define SPELL_WORD_ISNOT_CORRECT -1
#define SPELL_WORD_IS_CORRECT     0
#define SPELL_WORD_IS_IGNORED     1

#define SPELL_OK                  0
#define SPELL_ERROR              -1

#define SPELL_MIN_WORD_LEN 4

#define SPELL_DONOT_CLEAR_DICTIONARY 0
#define SPELL_CLEAR_DICTIONARY       1

#define SPELL_DONOT_CLEAR_IGNORED 0

typedef Imap_t spelldic_t;

typedef struct spell_t {
  char
    word[MAXLEN_WORD];

  int
    retval;

  utf8 c;

  size_t
    word_len,
    num_dic_words,
    min_word_len;

  string_t
    *tmp,
    *dic_file;

  spelldic_t
    *dic,
    *ign_words;

  Vstring_t
    *words,
    *guesses,
    *messages;
} spell_t;

typedef struct spell_get_self {
  int (*num_entries) (void);
  string_t *(*dictionary) (void);
} spell_get_self;

typedef struct spell_set_self {
  void (*dictionary) (char *);
} spell_set_self;

typedef struct spell_self {
  spell_get_self get;
  spell_set_self set;

  spell_t *(*new) (void);

  void
    (*clear) (spell_t *, int),
    (*release) (spell_t *, int),
    (*add_word_to_dictionary) (spell_t *, char *);

  int
    (*correct) (spell_t *),
    (*init_dictionary) (spell_t *, string_t *, int, int);
} spell_self;

typedef struct spell_T {
  spell_self self;
  int num_entries;
  string_t *dic_file;
  spelldic_t *current_dic;
} spell_T;

public spell_T __init_spell__ (void);
public void __deinit_spell__ (spell_T *);

#endif /* SPELL_HDR */
