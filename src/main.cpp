#include <string>
#include <filesystem>

#include "online/Server.h"
#include "core/Window.h"
#include "core/User.h"
#include "game/context/GameWindowContent.h"
#include "util/OSCommandDependencies.h"

#undef main

using namespace std;

// int main( int argc, char *argv[] )
// {
//     if (argc == 2 && !strcmp(argv[1], "_server")) {
//         window.close();
//         user.close();
//
//         Server server(1998);
//         server.start();
//
//         return 0;
//     }
//
//
//     launchServerConsole(argv[0]);
//
//
//     // (32 x 40) x (32 x 24)
//     //player.setWindowContent(new MenuWindowContent());
//     //player.setWindowContent(new OptionWindowContent());
//     user.setWindowContent(new GameWindowContent());
//     user.start();
//     return EXIT_SUCCESS;
// }

int main(int argc, char *argv[])
{
//
//------------------------------------------------------------------------------------------------------
//                            MODE SERVEUR AUTONOME (multijoueur)
//------------------------------------------------------------------------------------------------------
//

    if (argc == 2 && !strcmp(argv[1], "_server")) {
        Server server(1998);
        server.start();

    }
//
//------------------------------------------------------------------------------------------------------
//                            MODE CLIENT AVEC SERVEUR INTERNE (solo)
//------------------------------------------------------------------------------------------------------
//

    else if (argc == 2 && !strcmp(argv[1], "_client")) {
        Server server(1998);
        std::thread serverThread([&server](){
            server.start();
        });

        user().setWindowContent(new GameWindowContent());
        user().start();

        server.stop();
        serverThread.join();
    }
//
//------------------------------------------------------------------------------------------------------
//                            MODE CLIENT AUTONOME (multijoueur)
//------------------------------------------------------------------------------------------------------
//

    else {
        user().setWindowContent(new GameWindowContent());
        user().start();
    }
//
//------------------------------------------------------------------------------------------------------
//                            SORTIE DU PROGRAMME
//------------------------------------------------------------------------------------------------------
//
    window().shutdown();
    return EXIT_SUCCESS;
}