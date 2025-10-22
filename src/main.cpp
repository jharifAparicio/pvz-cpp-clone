#include "core/Game.hpp"
#include <iostream>

int main() {
    Game game;
    if (!game.init("PVZ Clone - C++ SDL2", 1280, 720, true)) {
        std::cerr << "No se pudo iniciar el juego.\n";
        return 1;
    }
    game.run();
    return 0;
}
