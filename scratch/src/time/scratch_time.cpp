//
// Created by JJJai on 3/11/2021.
//

#include <GLFW/glfw3.h>
#include "scratch_time.h"

void scratch::Time::InitializeClock() {
    lastFrameTime = glfwGetTime();
    elapsedTime = 0;
}

void scratch::Time::UpdateClock() {
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrameTime;
    lastFrameTime = currentFrame;
    elapsedTime += deltaTime;
}

float scratch::Time::GetDeltaTime() {
    return deltaTime;
}

double scratch::Time::GetElapsedTime() {
    return elapsedTime;
}
