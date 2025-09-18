//
// Created by NavKav on 23/01/2023.
//

#include "Game.h"
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <utility>

#define DIST(p, q) (sqrt(pow(p-q, 2)))
#define ABS(x) ((x) < 0 ? -(x) : (x))

std::mt19937 g_perlin_gen(42);
std::uniform_real_distribution<> g_offset_dis(-1000.0, 1000.0);
std::vector<std::pair<double, double>> g_offsets;

using namespace std;

Game::Game() : _client(1998, "127.0.0.1") {
}

double computeNoiseHeight(PerlinNoise& perlinNoise, int x, int y, const std::vector<std::pair<double, double>>& offsets) {
    float scale = 10, persistence = 0.5, lacunarity = 2;
    int octave = 5;
    double tmp;

    float amplitude = 1;
    float frequency = 1.5;
    float noiseHeight = 0;

    for (unsigned int aux = 0; aux < octave; aux++) {
        double sx = (x / scale * frequency) + offsets[aux].first;
        double sy = (y / scale * frequency) + offsets[aux].second;
        tmp = perlinNoise.noise(sx, sy) * 2 - 1;
        noiseHeight += tmp * amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    double maxAmplitude = (1.0 * (1.0 - pow(persistence, octave))) / (1.0 - persistence);
    return noiseHeight / maxAmplitude;
}

void Game::process() {
    /*************************************************************************/
    /************************* INITIALIZATION  ******************************/
    /*************************************************************************/

    window.changeDrawColor(255, 0, 0, ALPHA_OPAQUE);
    window.clearBackground();

    // PerlinNoise perlinNoise(8);
    //
    // if (g_offsets.empty()) {
    //     g_offsets.resize(5);
    //     for (int i = 0; i < 5; ++i) {
    //         g_offsets[i] = {g_offset_dis(g_perlin_gen), g_offset_dis(g_perlin_gen)};
    //     }
    // }
    //
    // /*************************************************************************/
    // /************************* UNITS TESTS  *********************************/
    // /*************************************************************************/
    //
    // unsigned int X = window.getX() / 100, Y = window.getY() / 100;
    //
    // struct point {
    //     int x = -1, y = -1;
    //     int neighboor = 0;
    //     double height = 0.0;
    // };
    //
    // std::vector<std::vector<point>> t(X, std::vector<point>(Y));
    //
    // for (unsigned int x = 0; x < X; x++) {
    //     for (unsigned int y = 0; y < Y; y++) {
    //         point& p = t[x][y];
    //         p.x = x * 100;
    //         p.y = y * 100;
    //         p.height = computeNoiseHeight(perlinNoise, p.x, p.y, g_offsets);
    //     }
    // }
    //
    // window.changeDrawColor(255, 255, 255, ALPHA_OPAQUE);
    //
    // std::random_device rd;
    // std::mt19937 gen_conn(rd());
    // std::uniform_real_distribution<> dis_conn(0.0, 1.0);
    //
    // for (unsigned int x = 0; x < X; x++) {
    //     for (unsigned int y = 0; y < Y; y++) {
    //         point& p = t[x][y];
    //         double h1 = p.height;
    //
    //         for (int dx = -1; dx <= 1; dx++) {
    //             for (int dy = -1; dy <= 1; dy++) {
    //                 if (dx == 0 && dy == 0) continue;
    //
    //                 int nx = x + dx;
    //                 int ny = y + dy;
    //
    //                 if (nx >= 0 && nx < X && ny >= 0 && ny < Y) {
    //                     point& q = t[nx][ny];
    //                     double h2 = q.height;
    //
    //                     double height_threshold = 0.1;
    //                     double connection_probability = 0.5;
    //
    //                     if (ABS(h1 - h2) < height_threshold) {
    //                         if (dis_conn(gen_conn) < connection_probability) {
    //                             window.drawLine(p.x, p.y, q.x, q.y);
    //                             p.neighboor++;
    //                             q.neighboor++;
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    //
    // for (unsigned int x = 0; x < X; x += 1) {
    //     for (unsigned int y = 0; y < Y; y += 1) {
    //         point& p = t[x][y];
    //         char r = 64 + (rand() % 26);
    //         if (p.neighboor) {
    //             window.writeText(p.x - 10, p.y - 12, std::string(1, r));
    //         }
    //     }
    // }

    /*************************************************************************/
    /************************* DISPLAY **************************************/
    /*************************************************************************/
    //_frameRate.display(window);
    window.refresh();
    /*if (user[SDL_SCANCODE_SPACE].pressed) {
        _client.sendTCP("a appuye sur espace");
    }
    if (user[SDL_SCANCODE_Q].pressed) {
        _client.sendUDP("hello world");
    }*/

    auto gameMessage = _client.receiveTCP();
    if (gameMessage != nullptr) {
        cout << "messag reçu" << endl;
    }


}