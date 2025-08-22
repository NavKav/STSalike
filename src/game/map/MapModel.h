//
// Created by NavKav on 15/08/2025.
//

#ifndef MAPMODEL_H
#define MAPMODEL_H

#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include "util/PerlinNoise.h"

struct Point {
    int x = -1, y = -1;
    int neighbor = 0;
    double height = 0.0;
    int edges = 0;
};

class MapModel {
public :
    MapModel(unsigned int seed);
    Point getNode(int x, int y);

private :
    PerlinNoise _perlinNoise;
    std::vector<std::pair<double, double>> _offsets;

    unsigned int _seed;

    std::random_device _rd;
    std::mt19937 _randAlg;
    std::uniform_real_distribution<> _unifRealDistrib;
    double _heightThreshold = 0.1;
    double _connectionProbability = 0.5;

    double computeNoiseHeight(int x, int y);
};



#endif //MAPMODEL_H
