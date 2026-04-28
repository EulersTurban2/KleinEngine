#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform bool uIsHyperbolic;

out vec4 vFragPos;

void main() {
    vec4 pos;
    
    if (uIsHyperbolic) {
        // Map mesh vertices to hyperboloid
        float w = sqrt(1.0 + dot(aPos, aPos));
        pos = vec4(aPos, w);
        
        // Transform to View Space
        vFragPos = uView * uModel * pos;
        
        // Klein Projection for the vertex positions
        vec4 kleinPos = vec4(vFragPos.xyz / vFragPos.w, 1.0);
        gl_Position = uProj * kleinPos;
    } else {
        gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
    }
}