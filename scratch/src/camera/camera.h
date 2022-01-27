#pragma once

#include <glm/glm.hpp>

namespace scratch {
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float FIELD_OF_VIEW = 60.0f;
    const float ZOOM = 45.0f;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE = 100.0f;

    class Camera {
    public:

        // Constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = YAW, float pitch = PITCH) :
                _front(glm::vec3(0.0f, 0.0f, -1.0f)),
                _movementSpeed(SPEED),
                _mouseSensitivity(SENSITIVITY),
                _zoom(ZOOM),
                _nearPlane(NEAR_PLANE),
                _farPlane(FAR_PLANE),
                _fieldOfView(FIELD_OF_VIEW),
                _position(position),
                _worldUp(up),
                _yaw(yaw),
                _pitch(pitch){
            updateCameraVectors();
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix() const;

        glm::mat4 getProjectionMatrix() const;

        const glm::vec3 & getPosition() const;

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(CameraMovement direction, float deltaTime);

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset);

    private:
        // Camera Attributes
        glm::vec3 _position;
        glm::vec3 _front;
        glm::vec3 _up;
        glm::vec3 _right;
        glm::vec3 _worldUp;
        // Euler Angles
        float _yaw;
        float _pitch;
        // projection options
        float _nearPlane;
        float _farPlane;
        float _fieldOfView;
        // Camera options
        float _movementSpeed;
        float _mouseSensitivity;
        float _zoom;

        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors();
    };

}