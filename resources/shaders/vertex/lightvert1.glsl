#version 330 core

#include <klein>

layout(location = 0) in vec3 aPos;       // flat model position (Euclidean path)
layout(location = 5) in vec3 aPosHyper;  // geodesic position (hyperbolic path)

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform bool uIsHyperbolic;
uniform int uProjectionModel; // Engine::Math::HyperbolicProjection (Klein=0, Poincare=1)

out vec4 vFragPos;

void main() {
    if (uIsHyperbolic) {
        float w = sqrt(1.0 + dot(aPosHyper, aPosHyper));
        vec4 pos = vec4(aPosHyper, w);

        vFragPos = uView * uModel * pos;

        vec4 ballPos = vec4(hyperboloidToModel(vFragPos, uProjectionModel), 1.0);
        gl_Position = uProj * ballPos;
    } else {
        vFragPos = uView * uModel * vec4(aPos, 1.0);
        gl_Position = uProj * vFragPos;
    }
}