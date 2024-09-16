#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "event_handler.h"

class Camera : CursorListener, KeyHoldListener, FrameListener {
    public:
        Camera(glm::vec3 position, glm::vec3 direction = glm::vec3(1.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));

        Camera() : position(0.0, 0.0, 0.0), direction(1.0, 0.0, 0.0), up(0.0, 1.0, 0.0) {
            EventHandler::addListener((CursorListener*)this);
            EventHandler::addListener((KeyHoldListener*)this);
            EventHandler::addListener((FrameListener*)this);
            calcPitchYaw(this->direction);
        }

        glm::vec3 getPosition() const;
        void changePosition(glm::vec3 deltaPos);
        void setPosition(glm::vec3 position);

        glm::vec3 getDirection() const;
        void setDirection(glm::vec3 direction);
        void changeDirection(float deltaYaw, float deltaPitch);
        void setYaw(float yaw);
        void setPitch(float pitch);
        void calcPitchYaw(glm::vec3 direction);

        void moveForward(float distance);
        void moveRight(float distance);
        void moveUp(float distance);

        float getFov() const;
        void setFov(float fov);
        
        glm::mat4 viewMatrix();
        glm::mat4 projectionMatrix();
        void debug();

        virtual void onCursorMove(Cursor cursor) override;
        virtual void onKeyHold(const bool keysHeld[]) override;
        virtual void onFrameEnd() override;

    private:
        float fov = 90.0;
        float aspectRatio = 800.0 / 600.0;
        float sensitivity = 0.006;

        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 up;

        glm::vec2 dirOffset = glm::vec2(0.0);
        glm::vec3 posOffset = glm::vec3(0.0);

        float yaw;
        float pitch;

        void checkYaw();
        void checkPitch();

        void updateDirection();
};

#endif
