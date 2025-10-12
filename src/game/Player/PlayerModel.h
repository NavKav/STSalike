//
// Created by NavKav on 12/10/2025.
//

#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H



class PlayerModel {
    public :
        PlayerModel(int playerId, int x, int y):
        _playerId(_playerId),
        _X(x),
        _Y(y){}

    private :
        int _playerId = -1;

    int _X = 0, _Y = 0;
};



#endif //PLAYERMODEL_H
