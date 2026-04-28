#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; 

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform bool uIsHyperbolic;

out vec4 vFragPos;
out vec4 vNormal;
out vec2 vTexCoords; 

void main() {
    vTexCoords = aTexCoords; 

    if (uIsHyperbolic) {
        // 1. Map to Hyperboloid
        float w = sqrt(1.0 + dot(aPos, aPos));
        vec4 pos = vec4(aPos, w);
        
        // 2. Solve for the Normal's W-component (wn)
        float wn = dot(aPos, aNormal) / w;
        vec4 rawNormal = vec4(aNormal, wn);
        
        // 3. Minkowski Normalization
        float sqLen = dot(rawNormal.xyz, rawNormal.xyz) - (wn * wn);
        vec4 normM = rawNormal / sqrt(max(sqLen, 0.000001));

        // 4. Transform to View Space
        vFragPos = uView * uModel * pos;
        vNormal = uView * uModel * normM;

        // 5. Project for Rasterization ONLY
        vec4 kleinPos = vec4(vFragPos.xyz / vFragPos.w, 1.0);
        gl_Position = uProj * kleinPos;
    } else {
        // Standard Euclidean Path
        vFragPos = uView * uModel * vec4(aPos, 1.0);
        vNormal = uView * uModel * vec4(aNormal, 0.0);
        gl_Position = uProj * vFragPos;
    }
}