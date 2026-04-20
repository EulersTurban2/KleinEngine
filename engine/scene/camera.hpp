#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

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

        class Camera
        {
            public:
                Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 3.0f)) {
                    isHyperbolic = false;
                    
                    // Kamera počinje na koordinatnom početku 4D hiperboloida
                    lorentzState = glm::mat4(1.0f); 
                    
                    // Standardne početne vrednosti za Euklidski model
                    position = startPos;
                    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
                    yaw = -90.0f;
                    pitch = 0.0f;
                    
                    movementSpeed = 1.0f;
                    mouseSensitivity = 0.1f;
                    fov = 45.0f;

                    updateEuclideanVectors();
                }
                ~Camera() {}

                void toggleGeometry() {
                    isHyperbolic =!isHyperbolic;
                }

                // Dobijanje matrice pogleda (View Matrix) spremne za Shader
                glm::mat4 getViewMatrix();
                glm::mat4 getProjectionMatrix(float screenWidth, float screenHeight);
                
                void returnToOrigin();

                void processKeyboard(CameraMovement direction, float deltaTime);
                void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
                
                bool getIsHyperbolic() const;
            private:
                // Osnovni atributi EUKLIDSKE kamere
                glm::vec3 position;
                glm::vec3 front;
                glm::vec3 up;
                glm::vec3 right;
                glm::vec3 worldUp;

                // Atributi za hiperbolicku kameru
                bool isHyperbolic;
                glm::mat4 lorentzState;

                // Eulerovi uglovi
                float yaw;
                float pitch;

                // Opcije kamere
                float movementSpeed;
                float mouseSensitivity;
                float fov;

                void updateEuclideanVectors();
        };
    }
}

#endif