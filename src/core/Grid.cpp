#include "core/Grid.hpp"

Grid::Grid(int rows, int cols, int cellWidth, int cellHeight, int offsetX, int offsetY)
    : rows_(rows), cols_(cols), cellW_(cellWidth), cellH_(cellHeight),
      offsetX_(offsetX), offsetY_(offsetY) {}

SDL_Rect Grid::getCell(int row, int col) const {
    return {offsetX_ + col * cellW_, offsetY_ + row * cellH_, cellW_, cellH_};
}

void Grid::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            SDL_Rect cell = getCell(r, c);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}
