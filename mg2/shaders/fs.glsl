//#version 330 core
//in vec2 vNDC;
//out vec4 fragColor;
//
//uniform mat4 uMinv;
//uniform mat4 uPinv;
//uniform mat4 uP; 
//
//vec3 reconstructWorldRay(out vec3 rayOrigin) {
//    vec4 camView   = vec4(0, 0, 0, 1);
//    vec4 pixelView = uPinv * vec4(vNDC, 1.0, 1.0);
//    pixelView /= pixelView.w;
//
//    rayOrigin      = (uMinv * camView).xyz;
//    vec3 rayTarget = (uMinv * pixelView).xyz;
//    return normalize(rayTarget - rayOrigin);
//}
//
//float gridLine(vec2 p, float interval, float thickness) {
//    // Obliczamy odległość od linii w pikselach
//    vec2 grid = abs(fract(p / interval - 0.5) - 0.5) / fwidth(p / interval);
//    float line = min(grid.x, grid.y);
//    // 1.0 to szerokość linii w pikselach ekranowych
//    return 1.0 - smoothstep(thickness - 1.0, thickness + 1.0, line);
//}
//
///// Analityczny antyaliasing oparty na pochodnych ekranowych 
///// (ang. Analytic Anti-Aliasing using Screen-space Derivatives)
///// https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8
//vec4 drawGrid(vec3 ro, vec3 rd) {
//    float denom = rd.z;
//    //if (abs(denom) < 1e-5) return vec4(0);
//    float t = -ro.z / denom;
//    if (t < 0.0) return vec4(0);
//
//    vec2 p = (ro + t * rd).xy;
//    
//    // Stała grubość linii na ekranie (np. 1.5 piksela)
//    float major = gridLine(p, 1.5, 1.2) * (1.0 - smoothstep(50.0, 100.0, t));
//    float minor = gridLine(p, 0.5, 1.0) * (1.0 - smoothstep(50.0, 100.0, t));
//
//    if (major > 0.0) return vec4(vec3(0.35), major);
//    if (minor > 0.0) return vec4(vec3(0.2), minor);
//    
//    return vec4(0);
//}
//
//vec2 getPixel(vec3 ro, vec3 rd) {
//    float denom = rd.z;
//    if (abs(denom) < 1e-5) return vec2(0);
//    float t = -ro.z / denom;
//    if (t < 0.0) return vec2(0);
//
//    return (ro + t * rd).xy;
//}
//
//
//// Konwersja t (odległość w world space) - głębokość w NDC
//float worldDepthToNDC(vec3 ro, vec3 rd, float t) {
//    vec3 worldPos = ro + rd * t;
//    vec4 clipPos = uP * inverse(uMinv) * vec4(worldPos, 1.0);
//    return (clipPos.z / clipPos.w) * 0.5 + 0.5;
//}
//
//void main() {
//    vec3 ro, rd;
//    rd = reconstructWorldRay(ro);
//
//    float distGrid = (rd.z != 0.0) ? -ro.z / rd.z : 1e10;
//    if (distGrid < 0.0) distGrid = 1e10;
//
//    float distNear, distFar;
//    vec4 grid = drawGrid(ro, rd);
//    fragColor = mix(vec4(0.1, 0.1, 0.1, 1.0), grid, grid.a);
//     gl_FragDepth = (distGrid < 1e9)
//        ? worldDepthToNDC(ro, rd, distGrid)
//        : 1.0;
//}