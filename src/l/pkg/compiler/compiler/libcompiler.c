#define REQUIRE_ALLOC
#define REQUIRE_STDIO
#define REQUIRE_STRING
#define REQUIRE_STR_EQ
#define REQUIRE_MAP

#include <libc.h>
#include "../lex/tokens.h"
#include <lex.h>
#include <compiler.h>

#define COMPILER_ERROR LA_ERROR
#define COMPILER_OK    LA_OK

static int compiler_throw_error (Compiler *compiler, int err, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  sys_vsnprintf (compiler->unit->err_msg, MAXLEN_COMPILER_ERR_MSG + 1, fmt, ap);
  va_end(ap);
  compiler->unit->retval = err;
  return err;
}

static Compiler_Unit *compiler_init_compiler (Compiler_Unit *unit, Compiler *compiler) {
  compiler->beg = NULL;
  compiler->end = NULL;
  compiler->unit = unit;
  unit->compiler = compiler;
  return unit;
}

static void compiler_add_type (Compiler_Unit *unit, const char *name, int type) {
  map_set (unit->types, (char *) name, (void *) type, NULL);
  if (type > unit->max_id_type)
    unit->max_id_type = type;
}

static void compiler_add_std_types (Compiler_Unit *unit) {
  compiler_add_type (unit, "boolean", BOOLEAN_TYPE);
  compiler_add_type (unit, "int", INTEGER_TYPE);
  compiler_add_type (unit, "number", NUMBER_TYPE);
  compiler_add_type (unit, "string", STRING_TYPE);
  compiler_add_type (unit, "array", ARRAY_TYPE);
  compiler_add_type (unit, "map", MAP_TYPE);
  compiler_add_type (unit, "list", LIST_TYPE);
}

static Compiler_Unit *compiler_init_unit (Compiler_Unit *unit, compiler_opts opts) {
  unit->err_msg[0] = '\0';
  unit->retval = 0;
  unit->lex_unit = opts.lex_unit;
  unit->compiler = opts.compiler;

  unit->types = map_new (24);
  unit->max_id_type = 0;
  compiler_add_std_types (unit);

  unit->flags = opts.flags;
  unit->to_json = opts.to_json;
  unit->to_lang = opts.to_lang;
  return unit;
}

/********************************************************/

static chunkType *compiler_new_chunk (Compiler *compiler, chunkTypeId chunk_id, lex_type *atom, int *ldws) {
  chunkType *chunk = Alloc (sizeof (chunkType));
  chunk->chunk_id = chunk_id;
  chunk->next = NULL;
  chunk->beg = NULL;
  (void) atom;
  (void) ldws;
  (void) compiler;
  return chunk;
}

static int compiler_parse_chunk (Compiler *compiler, lex_type *atom) {

  if (atom->prev->token_id != TOKEN_LDWS)
    return compiler_throw_error (compiler, COMPILER_ERROR, "[DEBUG] awaiting a ldws token after const symbol_name");

  int ldws = atom->prev->token_len;

  chunkType *chunk = NULL;

tostderr ("%s %d\n", __func__, atom->token_id);

  switch (atom->token_id) {
    case TOKEN_CONST:
      chunk = compiler_new_chunk (compiler, CONST_ASSIGNMENT, atom->next, &ldws);
      break;
    case TOKEN_VAR:
      chunk = compiler_new_chunk (compiler, VAR_ASSIGNMENT, atom->next, &ldws);
      break;
    default:
      return compiler_throw_error (compiler, COMPILER_ERROR, "can not determinate chunk type");
   }

  return chunk == NULL ? COMPILER_ERROR : COMPILER_OK;
}

Compiler_Unit *compiler_parse_lex (Compiler_Unit *unit) {
  Lex_Unit *lex_unit = unit->lex_unit;
  Lex *lex = lex_unit->lex;
  Compiler *compiler = unit->compiler;

  lex_type *atom = lex_unit->beg->next;

  int retval;

  while (atom->token_id != TOKEN_EOF) {
    retval = compiler_parse_chunk (compiler, atom);
    if (retval == COMPILER_ERROR) break;
    atom = atom->next;
  }

  (void) lex_unit;
  (void) lex;

  return unit;
}

Compiler_Unit *compiler_parse (Compiler_Unit *unit) {
  lex_parse (unit->lex_unit);
  unit->retval = unit->lex_unit->retval;

  if (unit->retval) {
    sys_fprintf (sys_stderr, "%s\n", unit->lex_unit->err_msg);
    return unit;
  }

  if (unit->flags & COMPILER_DEBUG) {
    lex_print_lexemes (unit->lex_unit, unit->to_json, unit->to_lang);

    char *buf = NULL;
    ifnot (lex_validate_output (unit->lex_unit, &buf)) {
      sys_fprintf (sys_stderr, "%s\n", buf);
      sys_fprintf (sys_stdout, "# source unit and lex unit are not equal\n");
    } else
      sys_fprintf (sys_stdout, "# source unit and lex unit are equal\n");

     sys_fprintf (sys_stdout, "# NumTokens: %d, LineEnds: %d\n",
       unit->lex_unit->lex->num_tokens,
       unit->lex_unit->lex->lineNum);
  }

  return compiler_parse_lex (unit);
}

int compiler_parse_string (const char *src, compiler_opts opts) {
  Compiler_Unit unit;

  compiler_init_unit (&unit, opts);

  Compiler compiler;

  compiler_init_compiler (&unit, &compiler);

  Lex_Unit lex_unit;
  Lex lex;
  lex_unit.lex = &lex;
  lex_unit.src = src;
  lex_unit.unit_name = "<__string__>";

  unit.lex_unit = &lex_unit;

  compiler_parse (&unit);
  return 0;
}
