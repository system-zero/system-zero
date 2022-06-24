/*
  https://github.com/vaithak/Sudoku-Generator
  too many thanks
 */

#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE

#include <z/cenv.h>

#include <z/sudoku.h>

MODULE(sudoku)

static VALUE Sudoku_generate (la_t *this) {
  (void) this;
  sudoku_t sudoku;
  sudoku_init (&sudoku);
  sudoku_generate (&sudoku);
  sudoku_print (&sudoku);
  return OK_VALUE;
}

public int __init_sudoku_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "sudoku_generate",  PTR(Sudoku_generate), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sudoku = {
      generate : sudoku_generate
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_sudoku_module__ (la_t *this) {
  (void) this;
  return;
}
