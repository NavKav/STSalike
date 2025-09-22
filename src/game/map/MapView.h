//
// Created by NavKav on 20/09/2025.
//

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <unordered_map>
#include <memory>
#include "MapModel.h"


class MapView {
public :
    void addNode(int x, int y, Node node);
    Node* getNode(int x, int y);
    void displayMap();

private:
    std::unordered_map<Coords, std::unique_ptr<Node>, CoordsHash> _knownNodes;

};



#endif //MAPVIEW_H
