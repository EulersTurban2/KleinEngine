#ifndef PROJECTIONS_HPP
#define PROJECTIONS_HPP

#include <cmath>
#include <glm/glm.hpp>

namespace Engine::Math {

    // Projection models for displaying hyperbolic 3-space.
    enum class HyperbolicProjection {
        Klein = 0,
        Poincare = 1,
        HalfSpace = 2
    };

    // The hyperboloid (Minkowski) model is the canonical representation: a point
    // is X = (x, w) with signature (+,+,+,-), so |x|^2 - w^2 = -1 and
    // w = sqrt(1 + |x|^2), w >= 1. Every display model is just a flattening of
    // it. All conversions go *through* the hyperboloid (hub-and-spoke): there are
    // deliberately no direct model-to-model shortcuts, so the hyperboloid point
    // is the single source of truth. Cross-model conversion is done via convert().

    // Points sit just inside the ideal boundary (|v| = 1) after the inbound maps,
    // so |v|^2 is clamped below 1 to avoid dividing by (1 - |v|^2) -> 0. Mirrors
    // the 0.9999 guard used in the GLSL helpers.
    inline constexpr float BOUNDARY_CLAMP = 0.9999f;

    // ----- Klein spoke (gnomonic projection from the origin) -----

    // Scale X so its time component is 1.
    inline glm::vec3 hyperboloidToKlein(const glm::vec4& X) {
        return glm::vec3(X) / X.w;
    }

    inline glm::vec4 kleinToHyperboloid(const glm::vec3& k) {
        float s = glm::min(glm::dot(k, k), BOUNDARY_CLAMP);
        float w = 1.0f / std::sqrt(1.0f - s);
        return glm::vec4(k * w, w);
    }

    // ----- Poincare spoke (stereographic projection from the pole (0,0,0,-1)) -----

    // The line from the pole through X crosses the equatorial disk at
    // x / (w + 1). (w + 1 >= 2, so this is always safe.)
    inline glm::vec3 hyperboloidToPoincare(const glm::vec4& X) {
        return glm::vec3(X) / (X.w + 1.0f);
    }

    inline glm::vec4 poincareToHyperboloid(const glm::vec3& p) {
        float s = glm::min(glm::dot(p, p), BOUNDARY_CLAMP); // |p|^2
        float denom = 1.0f - s;
        return glm::vec4(2.0f * p / denom, (1.0f + s) / denom);
    }

    // ----- Upper half-space spoke -----
    //
    // Maps to { (a, b, h) : h > 0 } with the boundary plane at h = 0. Derived by
    // composing hyperboloid -> Poincare -> sphere inversion at the pole; it
    // simplifies (using x^2+y^2+z^2 = w^2-1) to a division by (w + z). The model
    // is UNBOUNDED: as w + z -> 0 the point recedes to infinity.

    inline glm::vec3 hyperboloidToHalfSpace(const glm::vec4& X) {
        float denom = glm::max(X.w + X.z, 1e-6f); // > 0 on the upper sheet
        return glm::vec3(X.x / denom, X.y / denom, 1.0f / denom);
    }

    inline glm::vec4 halfSpaceToHyperboloid(const glm::vec3& q) {
        float h = glm::max(q.z, 1e-6f);
        float r2 = q.x * q.x + q.y * q.y + h * h;
        return glm::vec4(q.x / h, q.y / h, (1.0f - r2) / (2.0f * h), (1.0f + r2) / (2.0f * h));
    }

    // ----- Hub dispatchers: hyperboloid <-> selected display model -----

    inline glm::vec3 hyperboloidToModel(const glm::vec4& X, HyperbolicProjection model) {
        switch (model) {
            case HyperbolicProjection::Poincare:  return hyperboloidToPoincare(X);
            case HyperbolicProjection::HalfSpace: return hyperboloidToHalfSpace(X);
            case HyperbolicProjection::Klein:
            default:                              return hyperboloidToKlein(X);
        }
    }

    inline glm::vec4 modelToHyperboloid(const glm::vec3& v, HyperbolicProjection model) {
        switch (model) {
            case HyperbolicProjection::Poincare:  return poincareToHyperboloid(v);
            case HyperbolicProjection::HalfSpace: return halfSpaceToHyperboloid(v);
            case HyperbolicProjection::Klein:
            default:                              return kleinToHyperboloid(v);
        }
    }

    // The only cross-model path: always pivots through the hyperboloid hub.
    inline glm::vec3 convert(const glm::vec3& v, HyperbolicProjection from, HyperbolicProjection to) {
        return hyperboloidToModel(modelToHyperboloid(v, from), to);
    }
}

#endif // PROJECTIONS_HPP
