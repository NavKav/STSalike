//
// Created by NavKav on 30/08/2025.
//

#ifndef PLAYER_H
#define PLAYER_H



class Player {
public :
    Player(int playerId, int x, int y):
    _playerId(_playerId),
    _X(x),
    _Y(y){}

private :
    int _playerId = -1;

    int _X = 0, _Y = 0;
};



#endif //PLAYER_H
