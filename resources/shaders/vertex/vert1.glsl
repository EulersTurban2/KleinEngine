#version 330 core

#include <klein>

layout (location = 0) in vec3 aPos;      // flat model position (Euclidean path)
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec3 aPosHyper; // geodesic position (hyperbolic path)

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform bool uIsHyperbolic;
uniform int uProjectionModel; // Engine::Math::HyperbolicProjection (Klein=0, Poincare=1)

out vec4 vFragPos;
out vec4 vNormal;
out vec2 vTexCoords; 

void main() {
    vTexCoords = aTexCoords; 

    if (uIsHyperbolic) {
        // 1. Map to Hyperboloid (geodesic-tessellated position)
        float w = sqrt(1.0 + dot(aPosHyper, aPosHyper));
        vec4 pos = vec4(aPosHyper, w);

        // 2. Solve for the Normal's W-component (wn)
        float wn = dot(aPosHyper, aNormal) / w;
        vec4 rawNormal = vec4(aNormal, wn);
        
        // 3. Minkowski Normalization
        float sqLen = dot(rawNormal.xyz, rawNormal.xyz) - (wn * wn);
        vec4 normM = rawNormal / sqrt(max(sqLen, 0.000001));

        // 4. Transform to View Space
        vFragPos = uView * uModel * pos;
        vNormal = uView * uModel * normM;

        // 5. Flatten the hyperboloid point into the selected model for
        //    rasterization ONLY (lighting/depth still use vFragPos directly).
        vec4 ballPos = vec4(hyperboloidToModel(vFragPos, uProjectionModel), 1.0);
        gl_Position = uProj * ballPos;
    } else {
        // Standard Euclidean Path
        vFragPos = uView * uModel * vec4(aPos, 1.0);
        vNormal = uView * uModel * vec4(aNormal, 0.0);
        gl_Position = uProj * vFragPos;
    }
}