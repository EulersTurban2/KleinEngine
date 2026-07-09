#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>

#include "core/keymap.hpp"
#include "core/logger.hpp"
#include "math/lorentz.hpp"

namespace Engine::Scene {

    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    struct CameraSettings {
        float movementSpeed = 2.5f;
        float mouseSensitivity = 0.1f;
        float fov = 45.0f;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    };

    class Camera {
        public:
            explicit Camera(const CameraSettings& settings = CameraSettings());
            ~Camera() = default;

            void processKeyboard(CameraMovement direction, float deltaTime);
            void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

            // Restores the state the camera was constructed with.
            void reset();

            void toggleGeometry() {
                mIsHyperbolic = !mIsHyperbolic;
                std::string geomType = mIsHyperbolic ? "Hyperbolic" : "Euclidean";
                LOG_INFO("Camera geometry toggled to: " + geomType);
            }

            // View matrix ready to be handed to a shader
            glm::mat4 getViewMatrix() const;
            glm::mat4 getProjectionMatrix(float screenWidth, float screenHeight) const;

            glm::vec3 getPosition() const;

            bool getIsHyperbolic() const;

            void handleInput(float deltaTime, const Core::Keymap& keymap);

        private:
            // Euclidean camera state
            glm::vec3 mFront;
            glm::vec3 mUp;
            glm::vec3 mRight;
            glm::vec3 mWorldUp;

            // Hyperbolic camera state
            glm::mat4 mLorentzState;

            // Euler angles
            float mYaw;
            float mPitch;

            // Camera options
            bool mIsHyperbolic;
            CameraSettings mSettings;
            const CameraSettings mInitialSettings;

            void updateEuclideanVectors();
    };
}

#endif // CAMERA_HPP
