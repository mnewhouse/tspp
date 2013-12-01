#include <vector>
#include <string>

#include "core/game.hpp"

#include <iostream>

using namespace ts;

int main(int, char**)
{
    std::cout.sync_with_stdio(false);

    core::Game game;
    game.main();
}