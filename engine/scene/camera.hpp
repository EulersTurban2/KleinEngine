#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include <string>

#include "core/logger.hpp"
#include "math/lorentz.hpp"

namespace Engine
{
    namespace Camera
    {

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

        class Camera
        {
            public:
                explicit Camera(const CameraSettings& settings = CameraSettings());
                ~Camera() {}

                void processKeyboard(CameraMovement direction, float deltaTime);
                void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
                void reset();
                void toggleGeometry() { 
                    isHyperbolic = !isHyperbolic; 
                    std::string geomType = isHyperbolic ? "Hyperbolic" : "Euclidean";
                    LOG_INFO("Camera geometry toggled to: " + geomType);
                }

                // Dobijanje matrice pogleda (View Matrix) spremne za Shader
                glm::mat4 getViewMatrix() const;
                glm::mat4 getProjectionMatrix(float screenWidth, float screenHeight) const;
                
                
                bool getIsHyperbolic() const;
            private:
                // Osnovni atributi EUKLIDSKE kamere
                glm::vec3 front;
                glm::vec3 up;
                glm::vec3 right;
                glm::vec3 worldUp;

                // Atributi za hiperbolicku kameru
                glm::mat4 lorentzState;
                
                // Eulerovi uglovi
                float yaw;
                float pitch;
                float roll;
                
                // Opcije kamere
                bool isHyperbolic;
                CameraSettings m_settings;

                void updateEuclideanVectors();
        };
    }
}

#endif