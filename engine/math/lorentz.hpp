#ifndef LORENTZ_HPP
#define LORENTZ_HPP

// Lorentz transformations used to move through hyperbolic space.
#include <cmath>
#include <glm/glm.hpp>

namespace Engine::Lorentz {

    inline constexpr float SPEED_OF_LIGHT = 1.0f;

    inline float minkowskiDot(const glm::vec4& a, const glm::vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z - a.w * b.w;
    }

    inline float gamma(float v) {
        float beta = v / SPEED_OF_LIGHT;
        return 1.0f / std::sqrt(1.0f - beta * beta);
    }

    // Boost along the x axis
    inline glm::mat4 lorentzBoostX(float v) {
        float beta = v / SPEED_OF_LIGHT;
        float g = gamma(v);

        glm::mat4 m(1.0f);

        m[0][0] =  g;
        m[0][3] = -g * beta;
        m[3][0] = -g * beta;
        m[3][3] =  g;

        return m;
    }

    // Boost along the y axis
    inline glm::mat4 lorentzBoostY(float v) {
        float beta = v / SPEED_OF_LIGHT;
        float g = gamma(v);

        glm::mat4 m(1.0f);

        m[1][1] =  g;
        m[1][3] = -g * beta;
        m[3][1] = -g * beta;
        m[3][3] =  g;

        return m;
    }

    // Boost along the z axis
    inline glm::mat4 lorentzBoostZ(float v) {
        float beta = v / SPEED_OF_LIGHT;
        float g = gamma(v);

        glm::mat4 m(1.0f);

        m[2][2] =  g;
        m[2][3] = -g * beta;
        m[3][2] = -g * beta;
        m[3][3] =  g;

        return m;
    }

    /**
     * Floating-point error accumulates as boosts and rotations are composed,
     * so the frame is re-orthonormalized against the Minkowski metric.
     */
    inline void normalize(glm::mat4& m) {
        glm::vec4 x = m[0]; // Right
        glm::vec4 y = m[1]; // Up
        glm::vec4 z = m[2]; // Forward
        glm::vec4 w = m[3]; // Camera position (time-like vector)

        // Step 1: normalize the time-like position (W) to Minkowski norm -1
        w = w / std::sqrt(-minkowskiDot(w, w));

        // Step 2: orthogonalize and normalize Z against W
        z = z + minkowskiDot(z, w) * w;
        z = z / std::sqrt(minkowskiDot(z, z));

        // Step 3: orthogonalize and normalize Y against W and Z
        y = y + minkowskiDot(y, w) * w;
        y = y - minkowskiDot(y, z) * z;
        y = y / std::sqrt(minkowskiDot(y, y));

        // Step 4: orthogonalize and normalize X against W, Z and Y
        x = x + minkowskiDot(x, w) * w;
        x = x - minkowskiDot(x, z) * z;
        x = x - minkowskiDot(x, y) * y;
        x = x / std::sqrt(minkowskiDot(x, x));

        // Write the corrected orthogonal columns back into the matrix
        m[0] = x;
        m[1] = y;
        m[2] = z;
        m[3] = w;
    }

    /**
     * Inverse of a Lorentz frame matrix (fast inversion via the Minkowski metric).
     */
    inline glm::mat4 inverse(const glm::mat4& state) {
        glm::mat4 view = glm::transpose(state);
        view[0][3] = -view[0][3];
        view[1][3] = -view[1][3];
        view[2][3] = -view[2][3];
        view[3][0] = -view[3][0];
        view[3][1] = -view[3][1];
        view[3][2] = -view[3][2];
        return view;
    }
}

#endif // LORENTZ_HPP
