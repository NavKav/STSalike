//
// Created by NavKav on 23/01/2023.
//

#ifndef ARPG_STORYBOARD_GAME_H
#define ARPG_STORYBOARD_GAME_H

#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include "core/Window.h"
#include "online/Client.h"
#include "util/FrameRate.h"
#include "online/Serializer.h"
#include "game/map/MapModel.h"
#include "game/map/MapView.h"

class GameView {
public :
    GameView();
    void process();
    ~GameView();
private :
    Client _client;

    FrameRate _frameRate;

    MapView _mapView;

    void processGameMessage();
    void processMessageNODEUPDATE(std::unique_ptr<GameMessage>& gameMessage);
};


#endif //ARPG_STORYBOARD_GAME_H
