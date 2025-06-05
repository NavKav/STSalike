//
// Created by NavKav on 23/01/2023.
//

#include "Game.h"

using namespace std;

#include <cmath>

#define DIST(p, q) (sqrt(pow(p-q, 2)))

Game::Game() :
_client(1998, "127.0.0.1") {

}

double computeNoiseHeight(PerlinNoise& _perlinNoise, int x, int y) {
    double scale = 90, persistence = 0.5f, lacunarity = 2.0f;
    int octave = 5;

    double amplitude = 1.0f;
    double frequency = 1.0f;
    double noiseHeight = 0.0f;

    double cx = 0.0f, cy = 0.0f;

    double dx = x - cx;
    double dy = y - cy;

    double r = sqrt(dx * dx + dy * dy);
    double theta = atan2(dy, dx);

    for (int i = 0; i < octave; ++i) {
        double sx = (r / scale) * frequency;
        double sy = (theta / (2.0f * M_PI)) * frequency;

        double tmp = _perlinNoise.noise(sx, sy) * 2.0f - 1.0f;
        noiseHeight += tmp * amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return noiseHeight;
}

void Game::process() {
    /*************************************************************************/
    /*************************  INDEX  ***************************************/
    /*************************************************************************/

    /*************************************************************************/
    /*************************  INITIALIZATION  ******************************/
    /*************************************************************************/
    srand(time(NULL));

    window.changeDrawColor(255, 0, 0, ALPHA_OPAQUE);
    window.clearBackground();

    PerlinNoise perlinNoise(15);

    /*************************************************************************/
    /*************************  UNITS TESTS  *********************************/
    /*************************************************************************/

    int X = window.getX(), Y = window.getY();

    struct point {
        int x = -1, y = -1, neighboor = 0;
    };

    point t[11][11];

    for (int x = 0; x <= 10 ; x += 1) {
        for (int y = 0; y <= 10; y += 1) {
            point& p = t[x][y];
            p.x = x*100;
            p.y = y*100;
        }
    }

    window.changeDrawColor(255, 255, 255, ALPHA_OPAQUE);

    for (int x = 0; x < 11; x++) {
        for (int y = 0; y < 11; y++) {
            point& p = t[x][y];
            double h1 = computeNoiseHeight(perlinNoise, p.x, p.y);

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < 11 && ny >= 0 && ny < 11) {
                        point& q = t[nx][ny];
                        float h2 = computeNoiseHeight(perlinNoise, q.x, q.y);

                        if (std::abs(h1 - h2) < 0.11) {
                            window.drawLine(p.x, p.y, q.x, q.y);
                            p.neighboor++;
                            q.neighboor++;
                        }
                    }
                }
            }
        }
    }

    for (int x = 0; x <= 10 ; x += 1) {
        for (int y = 0; y <= 10; y += 1) {
            point& p = t[x][y];
            char r = 64 + (rand()%26);
            if (p.neighboor) {
                window.writeText(p.x - 10, p.y - 12, string(1,r));
            }
        }
    }

    /*************************************************************************/
    /*************************  DISPLAY **************************************/
    /*************************************************************************/
    //_frameRate.display(window);
    window.refresh();
    if (user[SDL_SCANCODE_SPACE].pressed) {
        _client.sendTCP("a appuye sur espace");
    }
    if (user[SDL_SCANCODE_Q].pressed) {
        _client.sendUDP("a appuye sur A");
    }
    user.waitAnyKeyThenClose();
}