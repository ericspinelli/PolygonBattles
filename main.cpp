#include <SFML/Graphics.hpp>
#include "Game.h"

int main()
{
    Game g = Game("config.txt");
    g.run();

    return 0;
}