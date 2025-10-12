//
// Created by NavKav on 15/08/2025.
//

#ifndef MAPMODEL_H
#define MAPMODEL_H

#include <cmath>
#include <iostream>
#include <mutex>
#include <random>
#include <unordered_map>
#include <vector>
#include <utility>

#include <util/json.hpp>
#include "util/PerlinNoise.h"

struct Coords {
    int x, y;

    bool operator==(const Coords& other) const {
        return x == other.x && y == other.y;
    }
};

struct CoordsHash {
    std::size_t operator()(const Coords& c) const {
        std::size_t h1 = std::hash<int>{}(c.x);
        std::size_t h2 = std::hash<int>{}(c.y);
        return h1 ^ (h2 << 1);
    }
};

struct Node {
    int x = -1, y = -1;
    double height = 0.0;
    int edges = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Node, x, y, height, edges)

class MapModel {
public :
    static const Coords _neighborOffsets[8];

    MapModel(unsigned int seed);
    Node getNode(int x, int y);
    std::vector<Node> getAdjacentNodes(int x, int y);
    std::vector<Node> getInSightNodes(int x, int y);

private :
    PerlinNoise _perlinNoiseNode;
    std::mutex _perlinNoiseNodeMutex;

    std::vector<std::pair<double, double>> _offsets;
    std::mutex _offsetsMutex;

    unsigned int _seed;

    std::random_device _rd;

    double _heightThreshold = 0.020;
    double _connectionProbability = 0.7;

    std::unordered_map<Coords, Node, CoordsHash> _nodeCache;
    std::mutex _cacheMutex;

    double computeNodeHeight(int x, int y);
};

#endif //MAPMODEL_H