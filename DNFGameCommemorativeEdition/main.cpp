#include <stdio.h>
#include <string.h>
#include <cmath>

#include "gamewindow.hpp"
#include "game.hpp"

// Window dimensions
const GLint WIDTH = 1200, HEIGHT = 900;

int
main(int argc, char** argv)
{
    GameWindow::launch(argc, argv, new Game(), WIDTH, HEIGHT, "DNF Yeah!");
    return 0;
}