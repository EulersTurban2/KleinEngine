#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform bool uIsHyperbolic;

#include <klein>

void main() {
    vec4 pos;
    
    if (uIsHyperbolic) {
        float w = sqrt(1.0 + aPos.x * aPos.x + aPos.y * aPos.y + aPos.z * aPos.z);
        pos = vec4(aPos, w);
    } else {
        pos = vec4(aPos, 1.0);
    }

    gl_Position = uProj * uView * uModel * pos;
}