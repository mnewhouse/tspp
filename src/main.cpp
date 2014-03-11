#include <vector>
#include <string>

#include "core/game.hpp"

#include <iostream>

using namespace ts;

#ifdef __linux__
#include <X11/Xlib.h>

void initialize()
{
    XInitThreads();
}
  
#else

void initialize()
{
}

#endif


int main(int, char**)
{
    initialize();
	
    std::cout.sync_with_stdio(false);

    core::Game game;
    game.main();
}
