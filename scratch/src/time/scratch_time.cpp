//
// Created by JJJai on 3/11/2021.
//

#include <GLFW/glfw3.h>
#include "scratch_time.h"

void scratch::Time::initializeClock() {
    _lastFrameTime = glfwGetTime();
    _elapsedTime = 0;
}

void scratch::Time::updateClock() {
    double currentFrame = glfwGetTime();
    _deltaTime = currentFrame - _lastFrameTime;
    _lastFrameTime = currentFrame;
    _elapsedTime += _deltaTime;
}

float scratch::Time::getDeltaTime() {
    return _deltaTime;
}

double scratch::Time::getElapsedTime() {
    return _elapsedTime;
}
