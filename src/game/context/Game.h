//
// Created by NavKav on 23/01/2023.
//

#ifndef ARPG_STORYBOARD_GAME_H
#define ARPG_STORYBOARD_GAME_H

#include <online/Client.h>
#include "core/User.h"
#include "util/FrameRate.h"
#include "util/PerlinNoise.h"

class Game {
public :
    Game();
    void process();
private :

    Client _client;

    PerlinNoise _perlinNoise;

    FrameRate _frameRate;
};


#endif //ARPG_STORYBOARD_GAME_H
