//
// Created by NavKav on 04/06/2021.
//
/*
 * Player source of inputs Class. Manage events from the launching computer.
 * Shall not use thread according to SDL function SDL_PollEvent() documentation.
 * */

#ifndef PROJECTTT_PLAYER_H
#define PROJECTTT_PLAYER_H

#include <ctime>
#include "WindowContent.h"

class User;
User& user();

#define NUM_SDLK 1000 // maximum range of SDL key number
#define SDL_SCANCODE_RIGHTCLICK NUM_SDLK - 1
#define SDL_SCANCODE_LEFTCLICK NUM_SDLK - 2

struct Input {
    bool pressed;
    long int u,v;
    Sint32 mouseX = 0, mouseY = 0;
};

class User {
public :
    User(const User&) = delete;
    User& operator=(const User&) = delete;

    // Méthodes publiques de la classe
    void start();
    void stop();
    ~User();
    void getMousePosition(int& x, int& y);
    Input operator[](unsigned int i);
    void setWindowContent(WindowContent *windowContent);
    void waitAnyKey();
    void waitAnyKeyThenClose();
    void close();

private :
    bool _boolLoop = true;
    Input _inputArr[NUM_SDLK] = {{false, 0, 0}};
    bool takeInput(const SDL_Event& event);
    unsigned int _numberKeyDown = 0;
    WindowContent* _windowContent{};

    friend User& user();
    User();
    static User& getInstance();
};

#endif //PROJECTTT_PLAYER_H