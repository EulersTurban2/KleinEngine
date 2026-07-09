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

    // ----- Hub dispatchers: hyperboloid <-> selected display model -----

    // HalfSpace is not implemented yet; it falls through to Klein for now.
    inline glm::vec3 hyperboloidToModel(const glm::vec4& X, HyperbolicProjection model) {
        switch (model) {
            case HyperbolicProjection::Poincare: return hyperboloidToPoincare(X);
            case HyperbolicProjection::Klein:
            case HyperbolicProjection::HalfSpace: // tmp resorts to Klein until HalfSpace is implemented
            default:                             return hyperboloidToKlein(X);
        }
    }

    inline glm::vec4 modelToHyperboloid(const glm::vec3& v, HyperbolicProjection model) {
        switch (model) {
            case HyperbolicProjection::Poincare: return poincareToHyperboloid(v);
            case HyperbolicProjection::Klein:
            case HyperbolicProjection::HalfSpace: // tmp resorts to Klein until HalfSpace is implemented
            default:                             return kleinToHyperboloid(v);
        }
    }

    // The only cross-model path: always pivots through the hyperboloid hub.
    inline glm::vec3 convert(const glm::vec3& v, HyperbolicProjection from, HyperbolicProjection to) {
        return hyperboloidToModel(modelToHyperboloid(v, from), to);
    }
}

#endif // PROJECTIONS_HPP
