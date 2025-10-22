#pragma once
#include <SDL.h>
#include <vector>

class Grid {
public:
    Grid(int rows, int cols, int cellWidth, int cellHeight, int offsetX = 0, int offsetY = 0);
    void render(SDL_Renderer* renderer);

    SDL_Rect getCell(int row, int col) const;
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

private:
    int rows_, cols_;
    int cellW_, cellH_;
    int offsetX_, offsetY_;
};
