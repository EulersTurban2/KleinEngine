#include "camera.hpp"
#include "core/input_manager.hpp"

namespace Engine::Scene {

    using Input = Engine::Core::InputManager;

    Camera::Camera(const CameraSettings& settings)
        : mSettings(settings), mInitialSettings(settings)
    {
        reset();
    }

    glm::mat4 Camera::getProjectionMatrix(float screenWidth, float screenHeight) const {
        if (mIsHyperbolic) {
            glm::mat4 proj = glm::mat4(0.0f);
            float tanHalfFov = std::tan(glm::radians(mSettings.fov) / 2.0f);

            proj[0][0] = 1.0f / (tanHalfFov * (screenWidth / screenHeight));
            proj[1][1] = 1.0f / tanHalfFov;

            proj[2][2] = 0.5f;
            proj[3][2] = 0.5f;

            proj[2][3] = 0.0f;
            proj[3][3] = 1.0f;

            return proj;
        } else {
            return glm::perspective(glm::radians(mSettings.fov), screenWidth / screenHeight, 0.1f, 100.0f);
        }
    }

    glm::vec3 Camera::getPosition() const {
        return mSettings.position;
    }

    glm::mat4 Camera::getViewMatrix() const {
        if (mIsHyperbolic) {
            // Fast inversion via the Minkowski metric
            return Lorentz::inverse(mLorentzState);
        } else {
            // Standard Euclidean view
            return glm::lookAt(mSettings.position, mSettings.position + mFront, mUp);
        }
    }

    void Camera::reset() {
        mSettings = mInitialSettings;
        mYaw = -90.0f;
        mPitch = 0.0f;
        mIsHyperbolic = false;
        mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        updateEuclideanVectors();
        mLorentzState = glm::mat4(1.0f);
    }

    void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
        // Doubles as the Euclidean step and the hyperbolic boost rapidity, so
        // movementSpeed reads as "hyperbolic distance per second".
        float velocity = mSettings.movementSpeed * deltaTime;
        glm::mat4 boost(1.0f);
        if (direction == CameraMovement::FORWARD) {
            mSettings.position += mFront * velocity;
            boost = Lorentz::lorentzBoostZ(velocity);
        }
        if (direction == CameraMovement::BACKWARD) {
            mSettings.position -= mFront * velocity;
            boost = Lorentz::lorentzBoostZ(-velocity);
        }
        if (direction == CameraMovement::LEFT) {
            mSettings.position -= mRight * velocity;
            boost = Lorentz::lorentzBoostX(velocity);
        }
        if (direction == CameraMovement::RIGHT) {
            mSettings.position += mRight * velocity;
            boost = Lorentz::lorentzBoostX(-velocity);
        }
        if (direction == CameraMovement::UP) {
            mSettings.position += mUp * velocity;
            boost = Lorentz::lorentzBoostY(velocity);
        }
        if (direction == CameraMovement::DOWN) {
            mSettings.position -= mUp * velocity;
            boost = Lorentz::lorentzBoostY(-velocity);
        }
        // Apply the local transformation and re-normalize
        mLorentzState = mLorentzState * boost;
        Lorentz::normalize(mLorentzState);
    }

    void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
        xoffset *= mSettings.mouseSensitivity;
        yoffset *= mSettings.mouseSensitivity;

        // 1. Update the Euclidean state (Euler angles)
        mYaw += xoffset;
        mPitch += yoffset;

        if (constrainPitch) {
            if (mPitch > 89.0f) mPitch = 89.0f;
            if (mPitch < -89.0f) mPitch = -89.0f;
        }
        updateEuclideanVectors();

        // 2. Update the hyperbolic state (rotation matrix)
        glm::mat4 rot = glm::mat4(1.0f);
        // Rotate around the Y axis (left/right) and the X axis (up/down)
        rot = glm::rotate(rot, glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
        rot = glm::rotate(rot, glm::radians(yoffset), glm::vec3(1.0f, 0.0f, 0.0f));

        // Apply the rotation and re-normalize
        mLorentzState = mLorentzState * rot;
        Lorentz::normalize(mLorentzState);
    }

    // Recomputes the camera basis vectors from the current Euler angles
    void Camera::updateEuclideanVectors() {
        glm::vec3 f;
        f.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        f.y = sin(glm::radians(mPitch));
        f.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

        mFront = glm::normalize(f);
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        mUp = glm::normalize(glm::cross(mRight, mFront));
    }

    bool Camera::getIsHyperbolic() const {
        return mIsHyperbolic;
    }

    void Camera::handleInput(float deltaTime, const Core::Keymap& keymap) {
        glm::vec2 mouseDelta = Input::getMouseDelta();
        processMouseMovement(mouseDelta.x, mouseDelta.y);

        if (Input::isKeyDown(keymap.getKey("camera.forward"))) processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (Input::isKeyDown(keymap.getKey("camera.backward"))) processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (Input::isKeyDown(keymap.getKey("camera.left"))) processKeyboard(CameraMovement::LEFT, deltaTime);
        if (Input::isKeyDown(keymap.getKey("camera.right"))) processKeyboard(CameraMovement::RIGHT, deltaTime);
        if (Input::isKeyDown(keymap.getKey("camera.up"))) processKeyboard(CameraMovement::UP, deltaTime);
        if (Input::isKeyDown(keymap.getKey("camera.down"))) processKeyboard(CameraMovement::DOWN, deltaTime);
        if (Input::isKeyPressed(keymap.getKey("camera.toggle_geometry"))) toggleGeometry();
        if (Input::isKeyPressed(keymap.getKey("camera.reset"))) reset();
    }
}
