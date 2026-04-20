#include "camera.hpp"

namespace Engine
{
    namespace Camera
    {

        glm::mat4 Camera::getProjectionMatrix(float screenWidth, float screenHeight) {
            if (isHyperbolic) {
                glm::mat4 proj = glm::mat4(0.0f);
                float tanHalfFov = std::tan(glm::radians(fov) / 2.0f);
                
                proj[0][0] = 1.0f / (tanHalfFov * (screenWidth / screenHeight));
                proj[1][1] = 1.0f / tanHalfFov;
                
                proj[2][2] = 0.5f;
                proj[3][2] = 0.5f; 
                
                proj[2][3] = 0.0f; 
                proj[3][3] = 1.0f; 
                
                return proj;
            } else {
                return glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
            }
        }

        glm::mat4 Camera::getViewMatrix() {
            if (isHyperbolic) {
                // Brza inverzija uz pomoć metrike Minkovskog
                return Lorentz::inverse(lorentzState);
            } else {
                // Standardni Euklidski pogled
                return glm::lookAt(position, position + front, up);
            }
        }

        void Camera::returnToOrigin() {
            position = glm::vec3(0.0f, 0.0f, 3.0f);
            yaw = -90.0f;
            pitch = 0.0f;
            updateEuclideanVectors();
            lorentzState = glm::mat4(1.0f); 
        }

        void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
            float velocity = movementSpeed * deltaTime;
            glm::mat4 boost(1.0f);
            if (direction == CameraMovement::FORWARD)  {
                position += front * velocity;
                boost = Lorentz::lorentzBoostZ(velocity);
            }
            if (direction == CameraMovement::BACKWARD) {
                position -= front * velocity;
                boost = Lorentz::lorentzBoostZ(-velocity);
            }
            if (direction == CameraMovement::LEFT)     {
                position -= right * velocity;
                boost = Lorentz::lorentzBoostX(velocity);
            }
            if (direction == CameraMovement::RIGHT)    {
                position += right * velocity;
                boost = Lorentz::lorentzBoostX(-velocity);
            }
            if (direction == CameraMovement::UP)       {
                position += up * velocity;
                boost = Lorentz::lorentzBoostY(velocity);
            }
            if (direction == CameraMovement::DOWN)     {
                position -= up * velocity;
                boost = Lorentz::lorentzBoostY(-velocity);
            }
            // Primjenjujemo lokalnu transformaciju i normalizujemo
            lorentzState = lorentzState * boost;
            Lorentz::normalize(lorentzState);
        }
        
        void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
            xoffset *= mouseSensitivity;
            yoffset *= mouseSensitivity;

            // 1. Ažuriranje Euklidskog stanja (Eulerovi uglovi)
            yaw   += xoffset;
            pitch += yoffset;

            if (constrainPitch) {
                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;
            }
            updateEuclideanVectors();

            // 2. Ažuriranje Hiperboličnog stanja (Matrica rotacije)
            glm::mat4 rot = glm::mat4(1.0f);
            // Rotiramo oko Y ose (Levo/Desno) i X ose (Gore/Dole)
            rot = glm::rotate(rot, glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f)); 
            rot = glm::rotate(rot, glm::radians(yoffset), glm::vec3(1.0f, 0.0f, 0.0f));  
            
            // Primjenjujemo rotaciju i normalizujemo)
            lorentzState = lorentzState * rot;
            Lorentz::normalize(lorentzState);
        }

        // Funkcija za azuriranje vektora kamere na osnovu trenutnih Eulerovih uglova
        void Camera::updateEuclideanVectors() {
            glm::vec3 frontVec;
            frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            frontVec.y = sin(glm::radians(pitch));
            frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(frontVec);
            right = glm::normalize(glm::cross(front, worldUp));
            up    = glm::normalize(glm::cross(right, front));
        }

        bool Camera::getIsHyperbolic() const {
            return isHyperbolic;
        }
    }
}