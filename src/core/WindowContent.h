//
// Created by NavKav on 29/08/2021.
//

#ifndef DICE_GAME_PROJECT_WINDOWCONTENT_H
#define DICE_GAME_PROJECT_WINDOWCONTENT_H

#include "Input.h"
#include "Window.h"

class User;

class WindowContent {
public :
    virtual void process() = 0;
};


#endif //DICE_GAME_PROJECT_WINDOWCONTENT_H
