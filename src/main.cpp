#include <vector>
#include <string>

#include "core/game.hpp"

#include <iostream>

using namespace ts;

#ifdef __linux__
  #include <X11/Xlib.h>
#endif

int main(int, char**)
{
#ifdef __linux__
	XInitThreads();
#endif

	
    std::cout.sync_with_stdio(false);

    core::Game game;
    game.main();
}