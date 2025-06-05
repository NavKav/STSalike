#include <string>
#include <filesystem>

#include "online/Server.h"
#include "core/Window.h"
#include "game/context/GameWindowContent.h"
#include "game/menu/MenuWindowContent.h"
#include "util/OSCommandDependencies.h"

#undef main

using namespace std;

using namespace std::filesystem;


int main( int argc, char *argv[] )
{
    if (argc == 2 && !strcmp(argv[1], "_server")) {
        window.close();
        user.close();

        Server server(1998);
        server.start();

        return 0;
    }


    //launchServerConsole(argv[0]);


    // (32 x 40) x (32 x 24)
    //player.setWindowContent(new MenuWindowContent());
    //player.setWindowContent(new OptionWindowContent());
    user.setWindowContent(new MenuWindowContent());
    user.start();
    return 0;
}