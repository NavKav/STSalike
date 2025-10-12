//
// Created by NavKav on 20/09/2025.
//

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <unordered_map>
#include <memory>
#include <core/Window.h>
#include <bitset>
#include "MapModel.h"


class MapView {
public :
    void addNode(int x, int y, Node node);
    Node* getNode(int x, int y);
    void displayMap();
    void setCameraCenter(int cx, int cy);
    std::pair<int,int> getCameraCenter() const;


private:
    std::unordered_map<Coords, std::unique_ptr<Node>, CoordsHash> _knownNodes;
    int _camX = 0;
    int _camY = 0;
};



#endif //MAPVIEW_H
