//
// Created by NavKav on 20/09/2025.
//

#include "MapView.h"

#include <core/Window.h>

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
    int radius = 10;
    Coords origin = {0, 0};
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            Coords currentCoords = {origin.x + x, origin.y + y};
            int distance_squared = x*x + y*y;
            int X = window.getX(), Y = window.getY();

            if (distance_squared <= radius * radius) {
                auto it = _knownNodes.find(currentCoords);
                if (it != nullptr) {
                    Node foundNode = *(it->second);
                    window.writeText(X/2 + 25 * foundNode.x, Y/2 + 25 * foundNode.y, "N");
                }
            }
        }
    }
}
