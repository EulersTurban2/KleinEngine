#version 330 core
out vec4 FragColor;

in vec4 vFragPos;

// Fed from the material's lightSourceSpec (resources.json)
struct LightSourceSpec {
    vec3 color;
    float intensity;
    float emission_strength;
};

uniform bool uIsHyperbolic;
uniform LightSourceSpec uLightSourceSpec;

void main() {
    FragColor = vec4(uLightSourceSpec.color * uLightSourceSpec.intensity, 1.0);

    if (uIsHyperbolic) {
        float safeW = max(vFragPos.w, 1.0);
        float trueDist = acosh(safeW);
        gl_FragDepth = clamp(trueDist / 10.0, 0.0, 1.0);
    } else {
        gl_FragDepth = gl_FragCoord.z;
    }
}
