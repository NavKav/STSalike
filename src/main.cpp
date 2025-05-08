#include <online/Server.h>

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
    if (argc == 2 && !strcmp(argv[1], "_server")) {
        window.~Window();

        Server server(1998);
        server.start();

        return 0;
    }

    // (32 x 40) x (32 x 24)
    //player.setWindowContent(new MenuWindowContent());
    //player.setWindowContent(new OptionWindowContent());
    player.setWindowContent(new GameWindowContent());
    player.start();
    return 0;
}