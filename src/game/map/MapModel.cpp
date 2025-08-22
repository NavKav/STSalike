//
// Created by NavKav on 15/08/2025.
//

#include "MapModel.h"

MapModel::MapModel(unsigned int seed) :
_seed(seed),
_perlinNoise(_seed),
_randAlg(seed),
_unifRealDistrib(0.0, 1.0)
{
    _offsets.resize(5);
    std::uniform_real_distribution<> offset_dist(-1000.0, 1000.0);
    for (int i = 0; i < 5; ++i) {
        _offsets[i] = {offset_dist(_randAlg), offset_dist(_randAlg)};
    }
}


double MapModel::computeNoiseHeight(int x, int y) {
    float scale = 10, persistence = 0.5, lacunarity = 2;
    int octave = 5;
    double tmp;

    float amplitude = 1;
    float frequency = 1.5;
    float noiseHeight = 0;

    for (unsigned int aux = 0; aux < octave; aux++) {
        double sx = (x / scale * frequency) + _offsets[aux].first;
        double sy = (y / scale * frequency) + _offsets[aux].second;
        tmp = _perlinNoise.noise(sx, sy) * 2 - 1;
        noiseHeight += tmp * amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    double maxAmplitude = (1.0 * (1.0 - pow(persistence, octave))) / (1.0 - persistence);
    return noiseHeight / maxAmplitude;
}

Point MapModel::getNode(int x, int y) {
    Point p;
    p.x = x;
    p.y = y;
    p.height = computeNoiseHeight(x, y);

    int neighborIndex = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            double aux = computeNoiseHeight(x + i, y + j);
            if (abs(p.height - aux) < _heightThreshold && _unifRealDistrib(_randAlg) < _connectionProbability) {
                p.edges |= (1 << neighborIndex);
            }
            neighborIndex++;
        }
    }

    return p;
}