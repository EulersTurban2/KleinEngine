#version 330 core
out vec4 FragColor;

in vec4 vFragPos; // You'll need to pass this from vertex shader
uniform bool uIsHyperbolic;

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Pure white glow

    if (uIsHyperbolic) {
        // Ensure the light source object depth matches the world depth logic
        float safeW = max(vFragPos.w, 1.0);
        float trueDist = acosh(safeW);
        gl_FragDepth = clamp(trueDist / 10, 0.0, 1.0);
    } else {
        gl_FragDepth = gl_FragCoord.z;
    }
}