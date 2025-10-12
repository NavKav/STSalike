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


/* Dessine les images selon la hauteur du neoud correspondant */
static void drawImageFromHeight(float height, int px, int py) {
    if (height < 0.1f) {
        window().drawIMG(px, py, "image/star.png");
    } else {
        window().drawIMG(px, py, "image/tree.png");
    }
}

/* setter et getter de MapView */
void MapView::setCameraCenter(int cx, int cy)
{
    _camX = cx;
    _camY = cy;
}

std::pair<int,int> MapView::getCameraCenter() const
{
    return {_camX, _camY};
}

void MapView::displayMap() {

    /* paramètres */
    int radius = 70;
    int X = window().getX(), Y = window().getY();
    int halfX = X/2, halfY = Y/2;
    const int tile = 50; /* espace entre les noeuds ?*/
    const int spriteHalf = 16; /* taille de l'image */
    const int camX = getCameraCenter().first;
    const int camY = getCameraCenter().second;
    Coords origin = {camX/tile, camY/tile};


    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            Coords currentCoords = {origin.x + x, origin.y + y};
            int distance_squared = x*x + y*y;

            if (distance_squared <= radius * radius) {
                auto it = _knownNodes.find(currentCoords);

                if (it == _knownNodes.end() || !it->second) {
                    continue;
                }

                Node foundNode = *(it->second);
                if (!foundNode.edges) continue;

                int px = halfX + (tile * foundNode.x - camX) - spriteHalf;
                int py = halfY + (tile * foundNode.y - camY) - spriteHalf;

                drawImageFromHeight(foundNode.height, px, py);

                // ------------------------------------
                int cx = halfX + (tile * foundNode.x - camX);
                int cy = halfY + (tile * foundNode.y - camY);

                std::bitset<8> bits((uint8_t)foundNode.edges);
                for (int i = 0; i < (int)bits.size(); ++i) {
                    if (!bits.test(i)) continue;

                    int nx = halfX + (tile * foundNode.x + MapModel::_neighborOffsets[i].x - camX);
                    int ny = halfY + (tile * foundNode.y + MapModel::_neighborOffsets[i].y - camY);

                    window().drawLine(cx, cy, nx, ny);
                }
            }
        }
    }
}
