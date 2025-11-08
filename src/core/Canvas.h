//
// Created by NavKav on 15/10/2025.
//

#ifndef CANVAS_H
#define CANVAS_H

#include "SDL2/SDL.h"
#if defined(_WIN32)
    #include "SDL_image.h"
    #include "SDL_ttf.h"
#else
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
#endif

class Window;

class Canvas {
public :
private :
    friend class Window;
    Canvas(unsigned int w, unsigned int h, SDL_Texture* texture);
    ~Canvas();
    SDL_Texture* getTexture();
    unsigned int getW() const;
    unsigned int getH() const;

    unsigned int _W, _H;

    SDL_Texture* _texture = nullptr;
};



#endif //CANVAS_H
