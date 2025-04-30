//
// Created by navid on 23/01/2023.
//

#ifndef ARPG_STORYBOARD_GAME_H
#define ARPG_STORYBOARD_GAME_H

#include "core/Player.h"
#include "util/FrameRate.h"

class Game {
public :
    void process(Player& player, Window& window);
private :
    FrameRate _frameRate;
};


#endif //ARPG_STORYBOARD_GAME_H
