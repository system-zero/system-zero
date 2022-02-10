/* https://github.com/vaithak/Sudoku-Generator
 * see LICENSE within this directory
 * Many thanks.
 */

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "_sudoku.h"

using namespace std;

int genRandNum(int maxLimit);
int genRandNum(int maxLimit)
{
  return rand()%maxLimit;
}

void Sudoku::createSeed()
{

  /* Fill diagonal boxes to form:
      x | . | .
      . | x | .
      . | . | x
  */
  this->fillEmptyDiagonalBox(0);
  this->fillEmptyDiagonalBox(1);
  this->fillEmptyDiagonalBox(2);

  /* Fill the remaining blocks:
      x | x | x
      x | x | x
      x | x | x
  */

  this->solveGrid(); // TODO: not truly random, but still good enough because we generate random diagonals.

  for(int i = 0; i < 9; i++)
    for(int j = 0; j < 9; j++)
      this->solved[i][j] = this->grid[i][j];
}

Sudoku::Sudoku()
{
  this->difficultyLevel = 0;
  this->num_copies = NUM_COPIES;
  this->numIterations = 0;
  this->print_colors = false;
  this->print_solved = true;

  // Randomly shuffling the array of removing grid positions
  for(int i=0;i<81;i++)
  {
    this->gridPos[i] = i;
  }

  random_shuffle(this->gridPos, (this->gridPos) + 81, genRandNum);

  // Randomly shuffling the guessing number array
  for(int i=0;i<9;i++)
  {
    this->guessNum[i]=i+1;
  }

  random_shuffle(this->guessNum, (this->guessNum) + 9, genRandNum);

  // Initialising the grid
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      this->grid[i][j]=0;
    }
  }

  grid_status = true;
}
// END: Initialising


// START: Custom Initialising with grid passed as argument
Sudoku::Sudoku(string grid_str, bool row_major)
{
  if(grid_str.length() != 81)
  {
    grid_status=false;
    return;
  }

  // First pass: Check if all cells are valid
  for(int i=0; i<81; ++i)
  {
    int curr_num = grid_str[i]-'0';
    if(!((curr_num == UNASSIGNED) || (curr_num > 0 && curr_num < 10))) {
      grid_status=false;
      return;
    }

    if(row_major) grid[i/9][i%9] = curr_num;
    else          grid[i%9][i/9] = curr_num;
  }

  // Second pass: Check if all columns are valid
  for (int col_num=0; col_num<9; ++col_num)
  {
    bool nums[10]={false};
    for (int row_num=0; row_num<9; ++row_num)
    {
      int curr_num = grid[row_num][col_num];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Third pass: Check if all rows are valid
  for (int row_num=0; row_num<9; ++row_num)
  {
    bool nums[10]={false};
    for (int col_num=0; col_num<9; ++col_num)
    {
      int curr_num = grid[row_num][col_num];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Fourth pass: Check if all blocks are valid
  for (int block_num=0; block_num<9; ++block_num)
  {
    bool nums[10]={false};
    for (int cell_num=0; cell_num<9; ++cell_num)
    {
      int curr_num = grid[((int)(block_num/3))*3 + (cell_num/3)][((int)(block_num%3))*3 + (cell_num%3)];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Randomly shuffling the guessing number array
  for(int i=0;i<9;i++)
  {
    this->guessNum[i]=i+1;
  }

  random_shuffle(this->guessNum, (this->guessNum) + 9, genRandNum);

  grid_status = true;
}
// END: Custom Initialising


// START: Verification status of the custom grid passed
bool Sudoku::verifyGridStatus()
{
  return grid_status;
}
// END: Verification of the custom grid passed

void Sudoku::printSeparator () {
  if (this->print_colors)
    cout<<"\033[33m";

  for (int i = 0; i < this->num_copies; i++) {
    for (int j = 0; j < 19; j++) {
      if (j % 6)
        cout <<"—";
      else
        cout <<"+";
    }

    if (i + 1 < this->num_copies)
      cout <<"  ";
  }

  if (this->print_colors)
    cout<<"\033[m";

  cout<<endl;
}

void Sudoku::printGrid() {
  this->printSeparator();

  for(int i = 0; i < 9; i++) {
    for (int n = 0; n < this->num_copies; n++) {
      if (this->print_colors)
        cout<<"\033[33m|\033[m";
      else
        cout<<"|";

      for (int j = 0; j < 9; j++)  {
        if(grid[i][j] == 0)
          cout<<" ";
        else
           cout<<grid[i][j];

        if (this->print_colors == false || (j + 1) % 3)
          cout<<"|";
        else
          cout<<"\033[33m|\033[m";
      }

      if (n + 1 < this->num_copies)
        cout<<"  ";
    }

    cout<<endl;

    if (i == 2 || i == 5 || i == 8)
      this->printSeparator();
  }

  if (this->difficultyLevel) {
    cout<<"\nDifficulty of current sudoku(0 being easiest): "<<this->difficultyLevel;
    cout<<" Num iterations: "<<this->numIterations;
    cout<<endl;
  }
}

void Sudoku::printSolved() {
  if (!this->print_solved)
    return;

  int numcopies = this->num_copies;
  this->num_copies = 1;
  this->printSeparator();

  for(int i = 0; i < 9; i++) {
    if (this->print_colors)
      cout<<"\033[33m|\033[m";
    else
      cout<<"|";

    for (int j = 0; j < 9; j++)  {
      if (solved[i][j] == 0)
        cout<<" ";
      else
        cout<<solved[i][j];

      if (this->print_colors == false || (j + 1) % 3)
        cout<<"|";
      else
        cout<<"\033[33m|\033[m";
    }

    cout<<endl;

    if (i == 2 || i == 5 || i == 8)
      this->printSeparator();
  }

  this->num_copies = numcopies;
}

bool FindUnassignedLocation(int grid[9][9], int &row, int &col);
bool FindUnassignedLocation(int grid[9][9], int &row, int &col)
{
    for (row = 0; row < 9; row++)
    {
        for (col = 0; col < 9; col++)
        {
            if (grid[row][col] == UNASSIGNED)
                return true;
        }
    }

    return false;
}

bool UsedInRow(int grid[9][9], int row, int num);
bool UsedInRow(int grid[9][9], int row, int num)
{
    for (int col = 0; col < 9; col++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInCol(int grid[9][9], int col, int num);
bool UsedInCol(int grid[9][9], int col, int num)
{
    for (int row = 0; row < 9; row++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInBox(int grid[9][9], int boxStartRow, int boxStartCol, int num);
bool UsedInBox(int grid[9][9], int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (grid[row+boxStartRow][col+boxStartCol] == num)
                return true;
        }
    }

    return false;
}

bool isSafe(int grid[9][9], int row, int col, int num);
bool isSafe(int grid[9][9], int row, int col, int num)
{
    return !UsedInRow(grid, row, num) && !UsedInCol(grid, col, num) && !UsedInBox(grid, row - row%3 , col - col%3, num);
}

// END: Helper functions for solving grid

void Sudoku::fillEmptyDiagonalBox(int idx)
{
  int start = idx*3;
  random_shuffle(this->guessNum, (this->guessNum) + 9, genRandNum);
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      this->grid[start+i][start+j] = guessNum[i*3+j];
    }
  }
}

// START: Modified Sudoku solver
bool Sudoku::solveGrid()
{
    this->numIterations++;

    int row, col;

    // If there is no unassigned location, we are done
    if (!FindUnassignedLocation(this->grid, row, col)) {

        if (this->print_solved) {
        this->num_copies = 1;
        this->printGrid();
        this->num_copies = NUM_COPIES;
      }

      return true; // success!
    }

    // Consider digits 1 to 9
    for (int num = 0; num < 9; num++)
    {
        // if looks promising
        if (isSafe(this->grid, row, col, this->guessNum[num]))
        {
            // make tentative assignment
            this->grid[row][col] = this->guessNum[num];

            // return, if success, yay!
            if (solveGrid())
                return true;

            // failure, unmake & try again
            this->grid[row][col] = UNASSIGNED;
        }
    }

    return false; // this triggers backtracking

}
// END: Modified Sudoku Solver


// START: Check if the grid is uniquely solvable
void Sudoku::countSolved(int &number)
{
  int row, col;

  if(!FindUnassignedLocation(this->grid, row, col))
  {
    number++;
    return ;
  }


  for(int i=0;i<9 && number<2;i++)
  {
      if( isSafe(this->grid, row, col, this->guessNum[i]) )
      {
        this->grid[row][col] = this->guessNum[i];
        countSolved(number);
      }

      this->grid[row][col] = UNASSIGNED;
  }

}
// END: Check if the grid is uniquely solvable


// START: Gneerate puzzle
void Sudoku::genPuzzle()
{
  for(int i=0;i<81;i++)
  {
    int x = (this->gridPos[i])/9;
    int y = (this->gridPos[i])%9;
    int temp = this->grid[x][y];
    this->grid[x][y] = UNASSIGNED;

    // If now more than 1 solution , replace the removed cell back.
    int check=0;
    countSolved (check);
    if(check!=1)
    {
      this->grid[x][y] = temp;
    }
  }
}
// END: Generate puzzle


// START: Calculate branch difficulty score
int Sudoku::branchDifficultyScore()
{
   int emptyPositions = -1;
   int tempGrid[9][9];
   int sum=0;

   for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      tempGrid[i][j] = this->grid[i][j];
    }
  }

   while(emptyPositions!=0)
   {
     vector<vector<int> > empty;

     for(int i=0;i<81;i++)
     {
        if(tempGrid[(int)(i/9)][(int)(i%9)] == 0)
        {
           vector<int> temp;
    temp.push_back(i);
  
    for(int num=1;num<=9;num++)
    {
      if(isSafe(tempGrid,i/9,i%9,num))
      {
        temp.push_back(num);
      }
    }

    empty.push_back(temp);
        }

     }

     if(empty.size() == 0)
     {
       cout<<"Hello: "<<sum<<endl;
       return sum;
     }

     int minIndex = 0;

     int check = empty.size();
     for(int i=0;i<check;i++)
     {
       if(empty[i].size() < empty[minIndex].size())
    minIndex = i;
     }

     int branchFactor=empty[minIndex].size();
     int rowIndex = empty[minIndex][0]/9;
     int colIndex = empty[minIndex][0]%9;

     tempGrid[rowIndex][colIndex] = this->solved[rowIndex][colIndex];
     sum = sum + ((branchFactor-2) * (branchFactor-2)) ;

     emptyPositions = empty.size() - 1;
   }

   return sum;

}
// END: Finish branch difficulty score


// START: Calculate difficulty level of current grid
void Sudoku::calculateDifficulty()
{
  int B = branchDifficultyScore();
  int emptyCells = 0;

  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
  if(this->grid[i][j] == 0)
     emptyCells++;
    }
  }

  this->difficultyLevel = B*100 + emptyCells;
}
// END: calculating difficulty level

void Sudoku::setPrintColors (bool val) {
  this->print_colors = val;
}

void Sudoku::setPrintSolved (bool val) {
  this->print_solved = val;
}

void Sudoku::setGridColumn (int x, int y, int v) {
  this->grid[x][y] = v;
}

void Sudoku::setSolvedColumn (int x, int y, int v) {
  this->solved[x][y] = v;
}

void Sudoku::setDifficultyLevel (int v) {
  this->difficultyLevel = v;
}

void Sudoku::setNumIterations (int v) {
  this->numIterations = v;
}

int Sudoku::getGridColumn (int x, int y) {
  return this->grid[x][y];
}

int Sudoku::getSolvedColumn (int x, int y) {
  return this->solved[x][y];
}

int Sudoku::getDifficultyLevel () {
  return this->difficultyLevel;
}

int Sudoku::getNumIterations () {
  return this->numIterations;
}
