#ifndef PROJECTIONS_HPP
#define PROJECTIONS_HPP

namespace Engine::Math {

    // Projection models for displaying hyperbolic 3-space.
    enum class HyperbolicProjection {
        Klein = 0,
        Poincare = 1,
        HalfSpace = 2
    };
}

#endif // PROJECTIONS_HPP
