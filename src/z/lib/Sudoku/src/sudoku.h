#ifndef SUDOKU_HDR
#define SUDOKU_HDR

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sudoku_t {
  char grid[9][9];
  char solved[9][9];

  int
    gridPos[81],
    guessNum[9],
    num_copies,
    grid_status,
    print_colors,
    print_solved,
    numIterations,
    difficultyLevel;
} sudoku_t;

void sudoku_init (sudoku_t *);
void sudoku_generate (sudoku_t *);
void sudoku_print (sudoku_t *);
void sudoku_solve (sudoku_t *);

#ifdef __cplusplus
}
#endif

#endif /* SUDOKU_HDR */
