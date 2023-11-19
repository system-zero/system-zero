#define COMPILER_DEBUG    (1 << 0)
#define COMPILER_NO_DEBUG (1 << 1)

#define MAXLEN_COMPILER_ERR_MSG 511

enum {
  UNKNOWN_TYPE = -1,
  NULL_TYPE,
  BOOLEAN_TYPE,
  INTEGER_TYPE,
  NUMBER_TYPE,
  UFUNCTION_TYPE,
  CFUNCTION_TYPE,
  STRING_TYPE,
  ARRAY_TYPE,
  MAP_TYPE,
  LIST_TYPE,
  OBJECT_TYPE,
  FILEPTR_TYPE,
  FD_TYPE
};

enum {
  CONST_INS = 1,
  INDEX_OPEN_INS,
  INDEX_CLOSE_INS,
  INT_INS,
  CHAR_INS,
  TYPE_INS,
  SYMBOL_INS
};

typedef struct Compiler_Unit Compiler_Unit;

typedef enum {
  CONST_ASSIGNMENT = 1,
  VAR_ASSIGNMENT,
} chunkTypeId;

typedef struct compilerAtom {
  int ins;
  int type;
  char *name;
  size_t namelen;
  struct compilerAtom *prev;
  struct compilerAtom *next;
} compilerAtom;

typedef struct chunkType {
  chunkTypeId chunk_id;
  compilerAtom  *beg;
  struct chunkType *next;
} chunkType;

typedef struct {
  chunkType *chunk_beg;
  compilerAtom *beg;
  compilerAtom *end;
  Compiler_Unit *unit;
} Compiler;

struct Compiler_Unit {
  char err_msg[MAXLEN_COMPILER_ERR_MSG + 1];
  int retval;

  Lex_Unit *lex_unit;
  Compiler *compiler;

  Map_Type *types;
  int max_id_type;

  int flags;
  int to_json;
  int to_lang;
};

typedef struct {
  int flags;
  int to_json;
  int to_lang;
  Lex_Unit *lex_unit;
  Compiler *compiler;
} compiler_opts;

#define CompilerOpts(...) (compiler_opts) { \
  .flags = COMPILER_DEBUG,                  \
  .to_json = 0,                             \
  .to_lang = 0,                             \
  .lex_unit = NULL,                         \
  .compiler = NULL,                         \
  __VA_ARGS__                               \
}

Compiler_Unit *compiler_parse_lex (Compiler_Unit *);
Compiler_Unit *compiler_parse (Compiler_Unit *);
int compiler_parse_string (const char *, compiler_opts);
