//
// Created by NavKav on 27/10/2022.
//

#ifndef ARPG_STORYBOARD_FRAMERATE_H
#define ARPG_STORYBOARD_FRAMERATE_H

#include <sys/time.h>
#include "core/Window.h"

#define GAP 5

class FrameRate {
public :
    FrameRate();
    void display();
private :
    time_t _lastTime;
    struct timeval _tp;
    int _previousValue = 0;
};

#endif //ARPG_STORYBOARD_FRAMERATE_H