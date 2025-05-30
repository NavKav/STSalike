//
// Created by NavKav on 11/06/2022.
//

#ifndef ARPG_STORYBOARD_GAMEWINDOWCONTENT_H
#define ARPG_STORYBOARD_GAMEWINDOWCONTENT_H

#include <cstdlib>

#include "core/WindowContent.h"
#include "game/context/Game.h"

class GameWindowContent : public WindowContent {
public :
    void process() override;
private :
    Game _game;

};


#endif //ARPG_STORYBOARD_GAMEWINDOWCONTENT_H
