#include "core/Window.h"
#include "game/context/GameWindowContent.h"
#include "game/menu/MenuWindowContent.h"

#undef main

using namespace std;

#include <string>
#include <filesystem>
using namespace std::filesystem;


int main( int argc, char *argv[] )
{
    // (32 x 40) x (32 x 24)
    player.setWindowContent(new MenuWindowContent());
    //player.setWindowContent(new OptionWindowContent());
    //player.setWindowContent(new GameWindowContent());
    player.start();
    return 0;
}