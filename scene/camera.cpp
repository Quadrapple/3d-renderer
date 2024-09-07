#include "camera.h"
#include "event_handler.h"

#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    this->position = position;
    this->direction = direction;
    this->up = up;
    this->fov = M_PIf / 2;

    calcPitchYaw(direction);

    EventHandler::addListener((CursorListener*)this);
    EventHandler::addListener((KeyHoldListener*)this);
    debug();
}


glm::vec3 Camera::getPosition() const {
    return position;
}

void Camera::changePosition(glm::vec3 deltaPos) {
    this->position += deltaPos; 
}

void Camera::setPosition(glm::vec3 position) {
    this->position = position;
}

void Camera::updateDirection() {
    direction.x = cos(yaw) * cos(pitch);
    direction.z = sin(yaw) * cos(pitch);
    direction.y = sin(pitch);

    direction = glm::normalize(direction);
}

glm::vec3 Camera::getDirection() const {
    return direction;
}

void Camera::setDirection(glm::vec3 direction) {
    this->direction = glm::normalize(direction);
    calcPitchYaw(this->direction);
}

void Camera::changeDirection(float deltaYaw, float deltaPitch) {
    pitch += deltaPitch;
    yaw += deltaYaw;

    checkYaw();
    checkPitch();

    updateDirection();
}

void Camera::calcPitchYaw(glm::vec3 direction) {
    pitch = asin(direction.y);
    yaw = atan2(direction.z, direction.x);
}

void Camera::setYaw(float yaw) {
    this->yaw = yaw;
    checkYaw();
}

void Camera::setPitch(float pitch) {
    this->pitch = pitch;
    checkPitch();
}

void Camera::checkYaw() {
    if(this->yaw > 2 * M_PIf) {
        this->yaw = 0;

    } else if(this->yaw < -2 * M_PIf) {
        this->yaw = 0;
    }
}

void Camera::checkPitch() {
    if(this->pitch > M_PIf / 2 - 0.001) {
        this->pitch = M_PIf / 2 - 0.001;

    } else if(this->pitch < -M_PIf / 2 + 0.001) {
        this->pitch = -M_PIf / 2 + 0.001;
    }
}

void Camera::moveForward(float distance) {
    glm::vec3 xzDirection = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    position += xzDirection * distance;
}

void Camera::moveRight(float distance) {
    position += glm::normalize(glm::cross(direction, up)) * distance;
}

void Camera::moveUp(float distance) {
    position += up * distance;
}

glm::mat4 Camera::viewMatrix() {
    //debug();
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::projectionMatrix() {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

float Camera::getFov() const {
    return fov;
}

void Camera::setFov(float fov) {
    this->fov = fov;
}

void Camera::onCursorMove(Cursor cursor) {
    float xOffset = cursor.pos.x - cursor.prevPos.x;
    float yOffset = cursor.prevPos.y - cursor.pos.y;

    changeDirection(xOffset * sensitivity, yOffset * sensitivity);
}

void Camera::onKeyHold(const bool keysHeld[]) {
    const float cameraSpeed = 4.0f * State::getContext().getDeltaTime();
    if(keysHeld[GLFW_KEY_W]) {
        moveForward(cameraSpeed);
    } else if(keysHeld[GLFW_KEY_S]) {
        moveForward(-cameraSpeed);
    }
    if(keysHeld[GLFW_KEY_D]) {
        moveRight(cameraSpeed);
    } else if(keysHeld[GLFW_KEY_A]) {
        moveRight(-cameraSpeed);
    }
    if(keysHeld[GLFW_KEY_SPACE]) {
        moveUp(cameraSpeed);
    } else if(keysHeld[GLFW_KEY_LEFT_SHIFT]) {
        moveUp(-cameraSpeed);
    }
}

void Camera::debug() {
    //std::cout << "POS: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    std::cout << "DIR: " << direction.x << ", " << direction.y << ", " << direction.z << " | YAW: " << yaw << ", PIT: " << pitch << std::endl;
}
