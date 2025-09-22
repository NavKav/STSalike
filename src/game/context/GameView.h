//
// Created by NavKav on 23/01/2023.
//

#ifndef ARPG_STORYBOARD_GAME_H
#define ARPG_STORYBOARD_GAME_H

#include <online/Client.h>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include "util/FrameRate.h"
#include "online/GlobalSerializer.h"
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
