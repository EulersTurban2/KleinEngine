float dotMinkowski(vec4 a, vec4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z - a.w*b.w;
}
vec4 normalizeMinkowski(vec4 v){
    float len = sqrt(max(0.0001,-dotMinkowski(v,v)));
    return v / len;
}

vec4 kleinToMinkowski(vec4 p){
    float s = p.x*p.x + p.y*p.y + p.z*p.z;
    float w = sqrt(1 / (1-s));
    return vec4(p.x*w,p.y*w,p.z*w,w);
}

vec4 projectToTangent(vec4 origin, vec4 target){
    return target + dotMinkowski(origin, target)*origin;
}

vec4 crossMinkowski(vec4 a, vec4 b, vec4 c){
    float nx =  determinant(mat3(a.yzw, b.yzw, c.yzw));
    float ny = -determinant(mat3(a.xzw, b.xzw, c.xzw));
    float nz =  determinant(mat3(a.xyw, b.xyw, c.xyw));
    float nw =  determinant(mat3(a.xyz, b.xyz, c.xyz));
    return vec4(nx,ny,nz,nw);
}

float distanceHyp(vec4 p1, vec4 p2) {
    return acosh(max(1.0, -dotMinkowski(p1, p2)));
}

float attenuationMinkowski(float dist, float radius){
    float sinh_d = sinh(dist);
    float decay = 1.0 / (1.0 + sinh_d*sinh_d);
    float ratio = dist / radius;
    float window = max(0.0,1.0-(ratio*ratio*ratio*ratio));
    return decay * window * window;
}
