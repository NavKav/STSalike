//
// Created by NavKav on 13/05/2021.
//
#include "Window.h"

#include "online/ServerConsole.h"

using namespace std;

Window& Window::getInstance() {
    // (32 x 40) x (32 x 24)
    static Window instance("STS Alike", 2000, 1200);
    return instance;
}

Window::Window(const string &windowName, unsigned int sizeX, unsigned int sizeY) :
_X(sizeX),
_Y(sizeY)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    _actualWindow = SDL_CreateWindow(windowName.c_str(),
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     (int) sizeX, (int) sizeY,  0);
    if (_actualWindow == nullptr) {
        serverConsole() << "Window::Window() :" << SDL_GetError()
                  << std::endl;
    }
    _renderer = SDL_CreateRenderer(_actualWindow, -1, SDL_RENDERER_ACCELERATED
                                                                        | SDL_RENDERER_PRESENTVSYNC
                                                                        | SDL_RENDERER_TARGETTEXTURE);

    //SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // check usefulness

    _windowTexture = SDL_CreateTexture(_renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET,
                                _X, _Y);
    SDL_SetTextureBlendMode(_windowTexture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(_renderer, _windowTexture);

    //SDL_SetWindowFullscreen(_actualWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    _font = TTF_OpenFont("ressource/font/arial.ttf", 25);
}

void Window::shutdown() {
    SDL_DestroyTexture(_windowTexture);

    for (auto d : _textureHashmap) {
        SDL_DestroyTexture(d.second);
    }
    _textureHashmap.clear();

    if (_font != nullptr) {
        TTF_CloseFont(_font);
        _font = nullptr;
    }

    if (_renderer != nullptr) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_actualWindow != nullptr) {
        SDL_DestroyWindow(_actualWindow);
        _actualWindow = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}

Window::~Window() {
    for (auto d : _textureHashmap) {
        if (d.second != nullptr) {
            SDL_DestroyTexture(d.second);
        }
    }
    _textureHashmap.clear();
}

void Window::setTitle(const std::string &windowTitle) {
    SDL_SetWindowTitle(_actualWindow, windowTitle.c_str());
}

void Window::drawIMG(int x, int y, const string &name) {
    open("ressource/" + name);
    drawIMG("ressource/" + name, x, y);
}

void Window::scaleIMG(int x, int y, int width, int height, const string &name) {
    string fullPath = "ressource/" + name;
    open(fullPath);

    auto it = _textureHashmap.find(fullPath);
    if (it == _textureHashmap.end()) {
        serverConsole() << "Window::scaleIMG() : Texture non trouvee pour " << name << endl;
        return;
    }
    SDL_Texture * texture = it->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = width;
    p.h = height;

    SDL_RenderCopy(_renderer, texture, NULL, &p);
}

void Window::refresh() {
    SDL_SetRenderTarget(_renderer, NULL);
    SDL_RenderCopy(_renderer, _windowTexture, NULL, NULL);

    SDL_RenderPresent(_renderer);

    SDL_SetRenderTarget(_renderer, _windowTexture);
    SDL_RenderClear(_renderer);

    //Sleep(20);
}

void Window::clear() {
    SDL_RenderClear(_renderer);
}

void Window::drawPartIMG(int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, const string &name) {
    open("ressource/" + name);
    drawPartIMG("ressource/" + name, x, y, a, b, c, d);
}

void Window::open(string file) {
    if (_textureHashmap.find(file) != _textureHashmap.end())
        return;

    SDL_Texture* texture = IMG_LoadTexture(_renderer, file.c_str());

    if (texture != NULL) {
        try {
            _textureHashmap.insert({file, texture});
        } catch (const std::bad_alloc& e) {
            serverConsole() << "Window::open() : ERREUR d'allocation memoire lors de l'insertion." << std::endl;
            SDL_DestroyTexture(texture);
        }
    } else {
        serverConsole() << "Window::open() : Echec du chargement de la texture : " << SDL_GetError() << endl;
    }
}

void Window::drawIMG(const string &name, int x, int y) {
    auto it = _textureHashmap.find(name);
    if (it == _textureHashmap.end()) {
        serverConsole() << "Window::drawIMG() : Texture non trouvee pour " << name << endl;
        return;
    }
    SDL_Texture* texture = it->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &(p.w), &(p.h));

    SDL_RenderCopy(_renderer, texture, NULL, &p);
}

void Window::drawPartIMG(const string &name, int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    auto it = _textureHashmap.find(name);
    if (it == _textureHashmap.end()) {
        serverConsole() << "Window::drawPartIMG() : Texture non trouvee pour " << name << endl;
        return;
    }
    SDL_Texture* texture = it->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = c;
    p.h = d;

    SDL_Rect r;
    r.x = a;
    r.y = b;
    r.w = c;
    r.h = d;

    SDL_RenderCopy(_renderer, texture, &r, &p);
}

void Window::changeFont(const string& name, unsigned int points) {
    TTF_CloseFont(_font);
    _font = TTF_OpenFont(("ressource/font/" + name + ".ttf").c_str(), points);
}

void Window::changeTextColor(Uint8 r, Uint8 v, Uint8 b) {
    _textColor = {r, v, b};
}

void Window::writeText(int x,int y, const string& s) {
    SDL_Surface * text = TTF_RenderText_Solid(_font, s.c_str(), _textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, text);

    SDL_Rect p;
    p.x = x;
    p.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &(p.w), &(p.h));

    SDL_RenderCopy(_renderer, texture, NULL, &p);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(texture);
}



unsigned int Window::getX() const {
    return _X;
}

unsigned int Window::getY() const {
    return _Y;
}

/* //!\\ SDL_GetRenderTarget wrong return bug isn't patched in SDL2 */
// void Window::shift(int x, int y) {
//     SDL_Rect p = {x, y, static_cast<int>(_X), static_cast<int>(_Y)};
//     SDL_Texture* currentTexture = SDL_GetRenderTarget(_renderer);
//
//     SDL_SetRenderTarget(_renderer, _auxTexture);
//     SDL_RenderCopy(_renderer, currentTexture, NULL, NULL);
//
//     SDL_SetRenderTarget(_renderer, currentTexture);
//     clear();
//     SDL_RenderCopy(_renderer, _auxTexture, NULL, &p);
// }

void Window::textSizeOf(std::string s, int& w, int& h) {
    TTF_SizeText(_font, s.c_str(), &w, &h);
}

bool Window::exists(std::string name) const {
    return !(_textureHashmap.find(name) == _textureHashmap.end());
}

void Window::drawLine(int a, int b, int c, int d) {
    SDL_RenderDrawLine(_renderer, a, b, c, d);
}

void Window::changeDrawColor(Uint8 r, Uint8 v, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(_renderer, r, v, b, a);
}

CanvasID Window::createCanvas(int w, int h) {
    SDL_Texture* sdlTexture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w,
        h
    );

    if (sdlTexture == nullptr) {
        serverConsole() << "Window::createCanvas : Impossible de créer une texte (" << w << "x" << h <<") : " << SDL_GetError();
        return -1;
    }

    SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
    SDL_Texture* previousTarget = SDL_GetRenderTarget(_renderer); // can be removed as we know that SDL_GetRenderTarget(_renderer) always equal to _windowTexture, tbm
    SDL_SetRenderTarget(_renderer, sdlTexture);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
    SDL_RenderClear(_renderer);
    SDL_SetRenderTarget(_renderer, previousTarget);

    _canvasIndex++;
    _canvasHashmap[_canvasIndex] = new Canvas(w, h, sdlTexture);

    return _canvasIndex - 1;
}

void Window::drawCanvas(int x, int y, CanvasID& canvasID) {
    auto it = _canvasHashmap.find(canvasID);
    if (it == _canvasHashmap.end()) {
        serverConsole() << "Window::drawIMG() : Canvas non trouvee pour " << canvasID << endl;
        return;
    }
    SDL_Texture* texture = it->second->_texture;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = it->second->_W;
    p.h = it->second->_H;

    SDL_RenderCopy(_renderer, texture, NULL, &p);
}

void Window::scaleCanvas(int x, int y, int width, int height, CanvasID& canvasID) {
    auto it = _canvasHashmap.find(canvasID);
    if (it == _canvasHashmap.end()) {
        serverConsole() << "Window::scaleIMG() : Canvas non trouvee pour " << canvasID << endl;
        return;
    }
    SDL_Texture * texture = it->second->_texture;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = it->second->_W;
    p.h = it->second->_H;

    SDL_RenderCopy(_renderer, texture, NULL, &p);
}

void Window::drawPartCanvas(int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, CanvasID& canvasID) {
    auto it = _canvasHashmap.find(canvasID);
    if (it == _canvasHashmap.end()) {
        serverConsole() << "Window::drawPartIMG() : Canvas non trouvee pour " << canvasID << endl;
        return;
    }
    SDL_Texture* texture = it->second->_texture;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = c;
    p.h = d;

    SDL_Rect r;
    r.x = a;
    r.y = b;
    r.w = c;
    r.h = d;

    SDL_RenderCopy(_renderer, texture, &r, &p);
}

void Window::drawOnCanvas(CanvasID canvasID, int x, int y, const string& name) {
    open("ressource/" + name);
    auto it = _textureHashmap.find(name);
    SDL_Texture* texture = it->second;

    auto itC = _canvasHashmap.find(canvasID);
    if (itC == _canvasHashmap.end()) {
        serverConsole() << "Window::drawOnCanvas() : canvasId non trouve pour " << canvasID << endl;
        return;
    }
    SDL_Texture* textureC = it->second;


    SDL_Rect p;
    p.x = x;
    p.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &(p.w), &(p.h));

    SDL_SetRenderTarget(_renderer, textureC);
    SDL_RenderCopy(_renderer, texture, NULL, &p);

    SDL_SetRenderTarget(_renderer, _windowTexture);
}

void Window::scaleOnCanvas(CanvasID& canvasID, int x, int y, int width, int height, const string& name) {
    open("ressource/" + name);
    auto it = _textureHashmap.find(name);
    SDL_Texture* texture = it->second;

    auto itC = _canvasHashmap.find(canvasID);
    if (itC == _canvasHashmap.end()) {
        serverConsole() << "Window::drawOnCanvas() : canvasId non trouve pour " << canvasID << endl;
        return;
    }
    SDL_Texture* textureC = it->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = width;
    p.h = height;

    SDL_SetRenderTarget(_renderer, textureC);
    SDL_RenderCopy(_renderer, texture, NULL, &p);

    SDL_SetRenderTarget(_renderer, _windowTexture);
}

void Window::drawPartOnCanvas(CanvasID& canvasID, int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, const string &name) {
    open("ressource/" + name);
    auto it = _textureHashmap.find(name);
    if (it == _textureHashmap.end()) {
        serverConsole() << "Window::drawPartIMG() : Texture non trouvee pour " << name << endl;
        return;
    }
    SDL_Texture* texture = it->second;

    auto itC = _canvasHashmap.find(canvasID);
    if (itC == _canvasHashmap.end()) {
        serverConsole() << "Window::drawOnCanvas() : canvasId non trouve pour " << canvasID << endl;
        return;
    }
    SDL_Texture* textureC = it->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    p.w = c;
    p.h = d;

    SDL_Rect r;
    r.x = a;
    r.y = b;
    r.w = c;
    r.h = d;

    SDL_SetRenderTarget(_renderer, textureC);
    SDL_RenderCopy(_renderer, texture, NULL, &p);

    SDL_SetRenderTarget(_renderer, _windowTexture);
}

void Window::writeTextOnCanvas(CanvasID& canvasID, int x, int y, const string& s) {
    SDL_Surface * text = TTF_RenderText_Solid(_font, s.c_str(), _textColor);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, text);

    auto itC = _canvasHashmap.find(canvasID);
    if (itC == _canvasHashmap.end()) {
        serverConsole() << "Window::drawOnCanvas() : canvasId non trouve pour " << canvasID << endl;
        return;
    }

    SDL_Texture* textureC = itC->second;

    SDL_Rect p;
    p.x = x;
    p.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &(p.w), &(p.h));

    SDL_RenderCopy(_renderer, texture, NULL, &p);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(texture);
}
