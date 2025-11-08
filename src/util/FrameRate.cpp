#include "FrameRate.h"

using namespace std;

FrameRate::FrameRate() {
    gettimeofday(&_tp, NULL);
    _lastTime = _tp.tv_sec * 1000 + _tp.tv_usec / 1000;
}

void FrameRate::display() {
    gettimeofday(&_tp, NULL);
    time_t currentTime = _tp.tv_sec * 1000 + _tp.tv_usec / 1000;
    
    time_t elapsedTime = currentTime - _lastTime;

    int newValue = 0;
    if (elapsedTime > 0) {
        newValue = static_cast<int>(1000.0 / static_cast<double>(elapsedTime));
    } else {
        newValue = _previousValue; // Ou une valeur par défaut, car la division est impossible
    }

    if (abs(newValue - _previousValue) < GAP) {
        newValue = _previousValue;
    }
    window().changeTextColor(255, 255, 255);
    window().changeFont("arial", 20);
    window().writeText(0, 0 , to_string(newValue) + " fps");

    _lastTime = currentTime;
    _previousValue = newValue;
}