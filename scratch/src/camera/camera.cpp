//
// Created by JJJai on 3/11/2021.
//

#include <glm/gtc/matrix_transform.hpp>

#include "main.h"

#include "camera.h"


// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 scratch::Camera::getViewMatrix() const {
    return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 scratch::Camera::getProjectionMatrix() const {
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    double aspectRatio = (width * 1.0) / (height * 1.0);
    return glm::perspective<double>(glm::radians(_fieldOfView), aspectRatio, _nearPlane, _farPlane);
}

const glm::vec3 &scratch::Camera::getPosition() {
    return _position;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void scratch::Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = _movementSpeed * deltaTime;
    if (direction == FORWARD)
        _position += _front * velocity;
    if (direction == BACKWARD)
        _position -= _front * velocity;
    if (direction == LEFT)
        _position -= _right * velocity;
    if (direction == RIGHT)
        _position += _right * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void scratch::Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void scratch::Camera::processMouseScroll(float yoffset) {
    if (_zoom >= 1.0f && _zoom <= 45.0f)
        _zoom -= yoffset;
    if (_zoom <= 1.0f)
        _zoom = 1.0f;
    if (_zoom >= 45.0f)
        _zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void scratch::Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(front,
                                       _worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up = glm::normalize(glm::cross(_right, front));
}