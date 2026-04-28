#version 330 core

#include <klein>

in vec4 vFragPos;
in vec4 vNormal; 
in vec2 vTexCoords; // NEW: Added UV input

uniform bool uIsHyperbolic;
uniform vec4 uLightPos; 
uniform vec3 uLightColor;
uniform vec4 cameraPos;
uniform float uLightRadius;

// NEW: The Albedo texture sampler
uniform sampler2D u_AlbedoMap; 

out vec4 FragColor;

void main() {
    vec3 finalColor = vec3(0.0);
    
    // Sample the albedo texture
    vec3 albedo = texture(u_AlbedoMap, vTexCoords).rgb;

    if (uIsHyperbolic) {
        vec4 P = normalizePositionM(vFragPos);
        float w = sqrt(1.0 + dot(uLightPos.xyz, uLightPos.xyz));
        vec4 safeLightPos = normalizePositionM(vec4(uLightPos.xyz, w));

        vec4 N = normalizeVectorM(projectToTangent(P, vNormal));
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 V = normalizeVectorM(projectToTangent(P, origin));
        vec4 L = normalizeVectorM(projectToTangent(P, safeLightPos));

        float dist = distanceHyp(P, safeLightPos);
        float attenuation = attenuationMinkowski(dist, 20 * uLightRadius);

        float diff = max(0.0, dotMinkowski(N, L));
        vec4 H = normalizeVectorM(L + V); 
        float spec = pow(max(0.0, dotMinkowski(N, H)), 32.0);
        
        finalColor = (diff + spec) * uLightColor * attenuation;

        float safeW = max(P.w, 1.0);
        float trueDist = acosh(safeW);
        gl_FragDepth = clamp(trueDist / 10.0, 0.0, 1.0);
    } else {
        vec3 P = vFragPos.xyz;
        vec3 N = normalize(vNormal.xyz);
        if (dot(N, P) > 0.0) { N = -N; }
        
        vec3 L = normalize(uLightPos.xyz - P);
        vec3 V = normalize(-P);
        
        float dist = length(uLightPos.xyz - P);
        float attenuation = clamp(1.0 - (dist / uLightRadius), 0.0, 1.0);
        attenuation *= attenuation; 
        
        float diff = max(dot(N, L), 0.0);
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), 32.0);
        
        finalColor = (diff + spec) * uLightColor * attenuation;
        gl_FragDepth = gl_FragCoord.z;
    }

    vec3 ambient = vec3(0.05); 
    
    // Final color combines ambient + dynamic lighting, multiplied by the texture albedo
    FragColor = vec4(albedo * (finalColor + ambient), 1.0);
}