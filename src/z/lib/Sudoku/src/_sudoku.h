#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#define UNASSIGNED 0
#define NUM_COPIES 3

class Sudoku {
private:
  int grid[9][9];
  int solved[9][9];
  int guessNum[9];
  int gridPos[81];
  int difficultyLevel;
  bool grid_status;

  int num_copies;     // addition
  int numIterations; // when solving grid
  bool print_colors;
  bool print_solved;

public:
  Sudoku ();
  Sudoku (std::string, bool row_major=true);
  void fillEmptyDiagonalBox(int);
  void createSeed();
  void printGrid();
  void printSolved();
  void printSeparator();
  bool solveGrid();
  std::string getGrid();
  void countSolved(int &number);
  void genPuzzle();
  bool verifyGridStatus();
  void calculateDifficulty();
  int  branchDifficultyScore();
  void setPrintColors(bool);
  void setPrintSolved(bool);
  void setGridColumn (int, int, int);
  void setSolvedColumn (int, int, int);
  void setDifficultyLevel (int);
  void setNumIterations (int);
  int  getSolvedColumn (int, int);
  int  getGridColumn (int, int);
  int  getDifficultyLevel ();
  int  getNumIterations ();

};

