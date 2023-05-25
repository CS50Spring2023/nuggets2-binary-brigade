// In player struct:
// initialized to false
bool[][] visible;
bool[][] known;

void
updateVisibility(grid_t* grid)
{
  bool visibility;
  for each point row, col in grid {
    visibility = lineCheck(grid, player->y_coord, player->x_coord, row, col);
    visible[row][col] = visible;
    if (visible == true) {
      known[row][col] = visible;
    }
  }
}
