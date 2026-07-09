#ifndef LORENTZ_HPP
#define LORENTZ_HPP

#include <cmath>
#include <glm/glm.hpp>

/**  
    Lorentz transformations used to move through hyperbolic space.

    Each projection of the hyperbolic space is practically a mapping from a 4D hyperboloid onto a 3D surface.
    The hyperboloid is a 4D surface defined by the equation:
        x^2 + y^2 + z^2 - w^2 = -1
    where (x, y, z) are the spatial coordinates and w is the time-like coordinate, with this we also equip ourselves with the Minkowski metric, 
    which is used to measure distances in this space. The metric is defined as follows:
        ds^2 = dx^2 + dy^2 + dz^2 - dw^2

    The Lorentz transformations are used to move through this hyperbolic space. We can think of them as a way to change our point of view in the hyperbolic space, 
    allowing us to move and rotate our perspective. The Lorentz transformations are represented by 4x4 matrices that operate on 4D vectors (x, y, z, w) in the hyperboloid model.
    The transformations include:
        - Boosts: These are transformations that change the velocity of the observer in a specific direction. 
          They are parameterized by rapidity, which is a measure of the "distance" moved in hyperbolic space. The rapidity is related to the velocity by the formula:
              beta = tanh(rapidity)
          where beta is the velocity as a fraction of the speed of light. The boosts are defined for each of the three spatial axes (x, y, z).
        - Rotations: These are transformations that rotate the observer's perspective around a specific axis.
    The Lorentz transformations preserve the Minkowski metric, meaning that the "distance" between points in hyperbolic space remains the same after applying a Lorentz transformation. This is crucial for maintaining
    the geometric properties of hyperbolic space as we move through it. The mathematical background can be found in the documenation of the Lorentz namespace (TODO: add link to documentation).


*/


namespace Engine::Lorentz {

    inline constexpr float SPEED_OF_LIGHT = 1.0f;

    inline float minkowskiDot(const glm::vec4& a, const glm::vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z - a.w * b.w;
    }

    // Geodesic midpoint of two points on the hyperboloid: the Minkowski-
    // normalized sum, i.e. the hyperbolic analog of the spherical midpoint
    // normalize(a + b). Equal to the hyperbolic-slerp evaluated at t = 1/2.
    inline glm::vec4 geodesicMidpoint(const glm::vec4& a, const glm::vec4& b) {
        glm::vec4 m = a + b;
        return m / std::sqrt(std::max(-minkowskiDot(m, m), 1e-12f));
    }

    inline float gamma(float v) {
        float beta = v / SPEED_OF_LIGHT;
        return 1.0f / std::sqrt(1.0f - beta * beta);
    }

    // The boosts below are parameterized by RAPIDITY (hyperbolic arc length),
    // not velocity beta. Rapidity is the natural "distance moved" parameter:
    // it adds linearly (boost(a)*boost(a) = boost(2a)), so movement is uniform
    // and frame-rate independent, and cosh/sinh stay finite at any speed (no
    // beta -> 1 singularity). Recover velocity via beta = tanh(rapidity).

    // Boost along the x axis by the given rapidity.
    inline glm::mat4 lorentzBoostX(float rapidity) {
        float ch = std::cosh(rapidity);
        float sh = std::sinh(rapidity);

        glm::mat4 m(1.0f);

        m[0][0] =  ch;
        m[0][3] = -sh;
        m[3][0] = -sh;
        m[3][3] =  ch;

        return m;
    }

    // Boost along the y axis by the given rapidity.
    inline glm::mat4 lorentzBoostY(float rapidity) {
        float ch = std::cosh(rapidity);
        float sh = std::sinh(rapidity);

        glm::mat4 m(1.0f);

        m[1][1] =  ch;
        m[1][3] = -sh;
        m[3][1] = -sh;
        m[3][3] =  ch;

        return m;
    }

    // Boost along the z axis by the given rapidity.
    inline glm::mat4 lorentzBoostZ(float rapidity) {
        float ch = std::cosh(rapidity);
        float sh = std::sinh(rapidity);

        glm::mat4 m(1.0f);

        m[2][2] =  ch;
        m[2][3] = -sh;
        m[3][2] = -sh;
        m[3][3] =  ch;

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
