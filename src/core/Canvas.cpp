//
// Created by NavKav on 15/10/2025.
//

#include "Canvas.h"

Canvas::Canvas(unsigned int w, unsigned int h, SDL_Texture* texture) :
_W(w),
_H(h),
_texture(texture)
{

}

SDL_Texture* Canvas::getTexture() {
    return _texture;
}

unsigned int Canvas::getW() const {
    return _W;
}

unsigned int Canvas::getH() const {
    return _H;
}

Canvas::~Canvas() {
    if (_texture)
        SDL_DestroyTexture(_texture);
}