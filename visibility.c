bool* updateVisibility(grid_t* grid, const int numRows, const int numCols, const int pr, const int pc);
bool lineCheck(grid_t* grid, const int pr, const int pc, const int row, const int col);

int
main()
{
  return 0
}

bool[][]
updateVisibility(grid_t* grid, const int numRows, const int numCols, const int pr, const int pc)
{
  bool visibility[numRows][numCols];

  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      visibility[row][col] = lineCheck(grid, pr, pc, row, col);
    }
  }
  return visibility;
}

bool 
lineCheck(grid_t* grid, const int pr, const int pc, const int row, const int col)
{
  float rowSlope;
  float colSlope;
  float currRow;
  float currCol;
  bool down;
  bool right;

  if (row >= pr) {
    down = true;
  }
  else {
    down = false;
  }
  if (col >= pc) {
    right = true;
  }
  else {
    right = false;
  }
  
  // check each row between player and point
  if (pr != row) {
    rowSlope = (col - pc)/(row - pr);
    if (down) {
      for (int r = pr + 1; r < row; r++) {
        currRow = r;
        currCol = pc + ((currRow - pr)*rowSlope);
        // col is an integer; check the intersect point
        if (floorf(currCol) == currColl) {
          //check currRow, currCol
        }
        // col not an integer; check points left and right
        else {
          //check currRow, floorf(currCol)
          //check currRow, ceilf(currCol)
        }
      }
    }
    else {
      for (int r = pr - 1; r > row; r--) {
        currRow = r;
        currCol = pc + ((currRow - pr)*rowSlope);
        // col is an integer; check the intersect point
        if (floorf(currCol) == currColl) {
          //check currRow, currCol
        }
        // col not an integer; check points left and right
        else {
          //check currRow, floorf(currCol)
          //check currRow, ceilf(currCol)
        }
      }
    }
  }
  // check each column between player and point
  if (pc != col) {
    colSlope = (row - pr)/(col - pc);
    if (right) {
      for (int c = pc + 1; c < col; c++) {
        currCol = c;
        currRow = pr + ((currCol - pc)*colSlope);
        // row is an integer; check the intersect point
        if (floorf(currRow) == currRow) {
          //check currRow, currCol
        }
        // row is not an integer; check points above and below
        else {
          //check floorf(currRow), currCol
          //check ceilf(currRow), currCol
        }
      }
    }
    else {
      for (int c = pc - 1; c > col; c--) {
        currCol = c;
        currRow = pr + ((currCol - pc)*colSlope);
        // row is an integer; check the intersect point
        if (floorf(currRow) == currRow) {
          //check currRow, currCol
        }
        // row is not an integer; check points above and below
        else {
          //check floorf(currRow), currCol
          //check ceilf(currRow), currCol
        }
      }
    }
  }
  return true;
}
