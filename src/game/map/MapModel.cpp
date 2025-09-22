//
// Created by NavKav on 15/08/2025.
//

#include "MapModel.h"

using namespace std;

const Coords MapModel::_neighborOffsets[8] = {
    {-1, 1},
    {0, 1},
    {1, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0}
};

MapModel::MapModel(unsigned int seed) :
_seed(seed),
_perlinNoiseNode(seed)
{
    _offsets.resize(5);
    uniform_real_distribution<> offset_dist(-1000.0, 1000.0);

    mt19937 temp_randAlg(seed);

    for (int i = 0; i < 5; ++i) {
        _offsets[i] = {offset_dist(temp_randAlg), offset_dist(temp_randAlg)};
    }
}


double MapModel::computeNodeHeight(int x, int y) {
    unique_lock<mutex> lock_perlin(_perlinNoiseNodeMutex);
    unique_lock<mutex> lock_offsets(_offsetsMutex);

    float scale = 10, persistence = 0.5, lacunarity = 2;
    int octave = 5;
    double tmp;

    float amplitude = 1;
    float frequency = 1.5;
    float noiseHeight = 0;

    for (unsigned int aux = 0; aux < octave; aux++) {
        double sx = (x / scale * frequency) + _offsets[aux].first;
        double sy = (y / scale * frequency) + _offsets[aux].second;
        tmp = _perlinNoiseNode.noise(sx, sy) * 2 - 1;
        noiseHeight += tmp * amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    double maxAmplitude = (1.0 * (1.0 - pow(persistence, octave))) / (1.0 - persistence);
    return noiseHeight / maxAmplitude;
}

Node MapModel::getNode(int x, int y) {
    Coords coords = {x, y};

    unique_lock<std::mutex> lock_cache(_cacheMutex);
    if (_nodeCache.contains(coords)) {
        return _nodeCache.at(coords);
    }

    Node p;
    p.x = x;
    p.y = y;

    p.height = computeNodeHeight(x, y);

    int neighborIndex = 0;
    for (int i = 0; i < 8; ++i) {
        Coords neighborCoords = {x + _neighborOffsets[i].x, y + _neighborOffsets[i].y};
        double neighborHeight;

        if (_nodeCache.contains(neighborCoords)) {
            neighborHeight = _nodeCache.at(neighborCoords).height;
        } else {
            neighborHeight = computeNodeHeight(x + _neighborOffsets[i].x, y + _neighborOffsets[i].y);
        }

        double edge_value = p.height + neighborHeight;

        if (std::abs(p.height - neighborHeight) < _heightThreshold && edge_value < _connectionProbability) {
            p.edges |= (1 << i);
        }
    }
    _nodeCache[coords] = p;
    return p;
}

vector<Node> MapModel::getAdjacentNodes(int x, int y) {
    vector<Node> adjacentPoints;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            adjacentPoints.push_back(getNode(x + i, y + j));
        }
    }
    return adjacentPoints;
}