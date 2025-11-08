//
// Created by NavKav on 13/05/2021.
//

#ifndef PROJECTTT_WINDOW_H
#define PROJECTTT_WINDOW_H

#include <string>
#include <unordered_map>
#include <iostream>
#include "SDL2/SDL.h"
#if defined(_WIN32)
    #include "SDL_image.h"
    #include "SDL_ttf.h"
#else
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
#endif

#define ALPHA_OPAQUE SDL_ALPHA_OPAQUE

class Window;
Window& window();

typedef std::string string;
typedef std::unordered_map<string, SDL_Texture*> hashmap;
typedef int CanvasID;

class Window {
public :
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    ~Window();
    void shutdown();

    void drawIMG(int x, int y, const string &name);
    void scaleIMG(int x, int y, int width, int height, const string &name);
    void drawPartIMG(int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, const string &name);
    void refresh();


    // REVOIR TOUTES LES FONCTIONS CANVAS
    CanvasID createCanvas(int w, int h);
    void drawOnCanvas(CanvasID canvasId, int x, int y, const string& name);
    void scaleOnCanvas(CanvasID& canvasID, int x, int y, int width, int height, const string& name);
    void drawPartOnCanvas(CanvasID& canvasID, int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, const string &name);
    void writeTextOnCanvas(CanvasID& canvasID, int x,int y, const string& s);
    void drawCanvas(int x, int y, CanvasID& canvasID);
    void scaleCanvas(int x, int y, int width, int height, CanvasID& canvasID);
    void drawPartCanvas(int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d, CanvasID& canvasID);

    void changeDrawColor(Uint8 r, Uint8 v, Uint8 b, Uint8 a);
    void clear();
    void drawLine(int a, int b, int c, int d);

    void writeText(int x,int y, const string& s);
    void changeFont(const string& name, unsigned int points);
    void changeTextColor(Uint8 r, Uint8 v, Uint8 b);
    void textSizeOf(std::string s, int& w, int& h);
    void setTitle(const string &windowTitle);

    bool exists(std::string name) const;
    unsigned int getY() const;
    unsigned int getX() const;

private :

    class Canvas {
    public :
        Canvas(unsigned int w, unsigned int h, SDL_Texture* texture) :
            _W(w), _H(h), _texture(texture) {}
        ~Canvas() {
            if (_texture)
                SDL_DestroyTexture(_texture);
        }
        unsigned int _W, _H;
        SDL_Texture* _texture = nullptr;
    };

    SDL_Texture* _windowTexture = nullptr;
    SDL_Window* _actualWindow;
    SDL_Renderer* _renderer;
    TTF_Font* _font;
    SDL_Color _textColor = {255, 255, 255 };

    hashmap _textureHashmap = hashmap();

    std::unordered_map<CanvasID, Canvas*> _canvasHashmap;
    int _canvasIndex = 0;

    unsigned int _X = 0, _Y = 0;

    friend Window& window();
    static Window& getInstance();
    Window(const string &windowName, unsigned int sizeX, unsigned int sizeY);
    void drawIMG(const string &name, int x, int y);
    void drawPartIMG(const string &name, int x, int y, unsigned int a, unsigned int b, unsigned int c, unsigned int d);
    void open(string file);

};

inline Window& window() {
    return Window::getInstance();
}

#endif //PROJECTTT_WINDOW_H