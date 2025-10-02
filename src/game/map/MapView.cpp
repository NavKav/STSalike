//
// Created by NavKav on 20/09/2025.
//

#include "MapView.h"

using namespace std;

void MapView::addNode(int x, int y, Node node) {
    _knownNodes[{x, y}] = std::make_unique<Node>(move(node));
}

Node* MapView::getNode(int x, int y) {
    auto it = _knownNodes.find({x, y});
    if (it != _knownNodes.end()) {
        return it->second.get();
    }
    return nullptr;
}

void MapView::displayMap() {
    int radius = 70;
    Coords origin = {0, 0};
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            Coords currentCoords = {origin.x + x, origin.y + y};
            int distance_squared = x*x + y*y;
            int X = window().getX(), Y = window().getY();

            if (distance_squared <= radius * radius) {
                auto it = _knownNodes.find(currentCoords);
                if (it != nullptr && it->second->edges) {
                    Node foundNode = *(it->second);
                    window().writeText(X/2 + 50 * foundNode.x - 7, Y/2 + 50 * foundNode.y - 7, "N");
                    window().changeDrawColor(0,0,0,255);
                    bitset<8> bits((uint8_t)foundNode.edges);
                    for (int i = 0; i < bits.size(); ++i) {
                        if (bits.test(i)) {
                            window().drawLine(X/2 + 50 * foundNode.x
                                              , Y/2 + 50 * foundNode.y
                                              , X/2 + 50 * (foundNode.x + MapModel::_neighborOffsets[i].x)
                                              , Y/2 + 50 * (foundNode.y + MapModel::_neighborOffsets[i].y)
                                              );
                        }
                    }
                }
            }
        }
    }
}
