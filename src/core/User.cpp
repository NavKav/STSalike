//
// Created by NavKav on 15/08/2021.
//

#include "User.h"

using namespace std;

User::User(Window& window, WindowContent* windowContent) : _windowContent(windowContent),
_window(window)
{
}

void User::start() {
    unsigned int i= 0;
    while(_boolLoop) {
        SDL_Event input;
        while(SDL_PollEvent(&input)){takeInput(input);}
        if (_windowContent) {_windowContent->process();}
    }
}

void User::stop() {
    _boolLoop = false;
}


User::~User() {
    if (_windowContent) {delete _windowContent;}
}

bool User::takeInput(const SDL_Event &event) {
    if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        stop();
        return true;
    }
    if (event.type == SDL_KEYDOWN) {
        if (!_inputArr[event.key.keysym.scancode].pressed)
            _inputArr[event.key.keysym.scancode] = {true, static_cast<long>(time(nullptr)), 0};
        return true;
    } else if (event.type == SDL_KEYUP) {
        if (_inputArr[event.key.keysym.scancode].pressed) {
            _inputArr[event.key.keysym.scancode].pressed = false;
            _inputArr[event.key.keysym.scancode].v = static_cast<long>(time(nullptr));
        }
        return true;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            if (!_inputArr[SDL_SCANCODE_RIGHTCLICK].pressed)
                _inputArr[SDL_SCANCODE_RIGHTCLICK] = {true, static_cast<long>(time(nullptr)), 0,  event.button.x, event.button.y};
            return true;
        } else { // SDL_BUTTON_LEFT
            if (!_inputArr[SDL_SCANCODE_LEFTCLICK].pressed)
                _inputArr[SDL_SCANCODE_LEFTCLICK] = {true, static_cast<long>(time(nullptr)), 0,  event.button.x, event.button.y};
            return true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            if (_inputArr[SDL_SCANCODE_RIGHTCLICK].pressed) {
                _inputArr[SDL_SCANCODE_RIGHTCLICK].pressed = false;
                _inputArr[SDL_SCANCODE_RIGHTCLICK].v = static_cast<long>(time(nullptr));
            }
            return true;
        } else { // SDL_BUTTON_LEFT
            if (_inputArr[SDL_SCANCODE_LEFTCLICK].pressed) {
                _inputArr[SDL_SCANCODE_LEFTCLICK].pressed = false;
                _inputArr[SDL_SCANCODE_LEFTCLICK].v = static_cast<long>(time(nullptr));
            }
            return true;
        }
    }
    return false;
}

Input User::operator[](unsigned int i) {
    return _inputArr[i];
}

void User::getMousePosition(int& x, int& y) {
    SDL_GetMouseState(&x, &y);
}

void User::setWindowContent(WindowContent *windowContent) {
    if (_windowContent) {delete _windowContent;}
    _windowContent = windowContent;
}

void User::waitAnyKey() {
    SDL_Event event;
    bool key_pressed = false;
    while (!key_pressed) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_KEYDOWN) {
            key_pressed = true;
        } else if (event.type == SDL_QUIT) {
            break;
        }
    }
}

void User::waitAnyKeyThenClose() {
    waitAnyKey();
    stop();
}

void User::close() {
    stop();
}


/*
void Player::start() {
    while(_boolLoop) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            Input input = takeInput(event);
            send(input);
        }

    }
}

Input Player::takeInput(const SDL_Event &event) {
    static std::set<SDL_KeyCode> pressedKeys;
    static std::set<Uint8> pressedButtons;
    Input input;

    if (event.type == SDL_KEYDOWN) {
        if (!pressedKeys.count(static_cast<SDL_KeyCode>(event.key.keysym.sym))) {
            input.key = static_cast<SDL_KeyCode>(event.key.keysym.sym);
            input.u = time(nullptr);
            _numberKeyDown++;
            pressedKeys.insert(static_cast<SDL_KeyCode>(event.key.keysym.sym));
        }
    } else if (event.type == SDL_KEYUP) {
        input.key = static_cast<SDL_KeyCode>(event.key.keysym.sym);
        input.v = time(nullptr);
        pressedKeys.erase(static_cast<SDL_KeyCode>(event.key.keysym.sym));

    } else if (event.type == SDL_MOUSEBUTTONDOWN ) {
        if (!pressedButtons.count(event.button.button)) {
            input.isMouseEvent = true;
            input.button = event.button.button;
            SDL_GetMouseState(reinterpret_cast<int *>(&(input.x)), reinterpret_cast<int *>(&(input.y)));
            input.u = time(nullptr);
        }
    } else if (event.type == SDL_MOUSEBUTTONUP ) {
        input.isMouseEvent = true;
        input.button = event.button.button;
        SDL_GetMouseState(reinterpret_cast<int *>(&(input.x)), reinterpret_cast<int *>(&(input.y)));
        input.v = time(nullptr);
        pressedButtons.erase(event.button.button);
    }

    if (event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT ) {
        stop();
    }
    return input;
}
*/
