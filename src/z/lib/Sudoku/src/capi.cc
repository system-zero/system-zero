
#include "sudoku.h"
#include "_sudoku.h"

extern "C" {
  void sudoku_init (sudoku_t *sudoku) {
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        sudoku->grid[i][j] = UNASSIGNED;
        sudoku->solved[i][j] = UNASSIGNED;
      }
    }

    sudoku->print_colors = 0;
    sudoku->print_solved = 1;
    sudoku->difficultyLevel = 0;
    sudoku->numIterations = 0;
    sudoku->num_copies = 3;
  }

  void sudoku_generate (sudoku_t *sudoku) {
    srand (time (NULL));

    Sudoku *puzzle = new Sudoku();

    puzzle->setPrintSolved (0);

    puzzle->createSeed();

    puzzle->genPuzzle();

    puzzle->calculateDifficulty();

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        sudoku->grid[i][j] = puzzle->getGridColumn (i, j);

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        sudoku->solved[i][j] = puzzle->getSolvedColumn (i, j);

    sudoku->difficultyLevel = puzzle->getDifficultyLevel ();
    sudoku->numIterations = puzzle->getNumIterations ();

    delete puzzle;
  }

  void sudoku_print (sudoku_t *sudoku) {
    Sudoku *puzzle = new Sudoku();

    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        puzzle->setGridColumn (i, j, sudoku->grid[i][j]);
        puzzle->setSolvedColumn (i, j, sudoku->solved[i][j]);
      }
    }

    puzzle->setPrintColors (sudoku->print_colors);
    puzzle->setPrintSolved (sudoku->print_solved);
    puzzle->setDifficultyLevel (sudoku->difficultyLevel);
    puzzle->setNumIterations (sudoku->numIterations);

    puzzle->printSolved ();
    puzzle->printGrid ();

    delete puzzle;
  }

  void sudoku_solve (sudoku_t *sudoku) {
    Sudoku *puzzle = new Sudoku ();
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        puzzle->setGridColumn (i, j, sudoku->grid[i][j]);

    puzzle->setPrintSolved (1);

    puzzle->solveGrid ();

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        sudoku->solved[i][j] = puzzle->getSolvedColumn (i, j);

    sudoku->difficultyLevel = puzzle->getDifficultyLevel ();

    delete puzzle;
  }

}
