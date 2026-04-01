#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 6) out;

in float vCenterPoint[];
in float vSelected[];
in float vHighlighted[];
in vec3  vWorldPos[];      
in float vCursor[];

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uM;
uniform float uAxisSize;

out vec3  gColor;
out float gSelected;
out float gHighlighted;
out float gIsLine;

void emitAxis(vec3 start, vec3 end, vec3 color) {
    mat4 PVM = uP * uV * uM;

    gColor       = color;
    gSelected    = vSelected[0];
    gHighlighted = vHighlighted[0];
    gIsLine      = 1.0; 
    gl_Position  = PVM * vec4(start, 1.0);
    EmitVertex();

    gl_Position  = PVM * vec4(end, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 wp = vWorldPos[0];

    if (vCenterPoint[0] > 0.5) {
        float s = uAxisSize * 0.8;
        emitAxis(wp, wp + vec3(s, 0, 0), vec3(0.8, 0.8, 0)); // X
        emitAxis(wp, wp + vec3(0, s, 0), vec3(0, 0.8, 0.8)); // Y
        emitAxis(wp, wp + vec3(0, 0, s), vec3(0.8, 0, 0.8)); // Z
    } else if(vCursor[0] > 0.5) {
        float s = uAxisSize;
        emitAxis(wp, wp + vec3(s, 0, 0), vec3(1, 0, 0)); // X
        emitAxis(wp, wp + vec3(0, s, 0), vec3(0, 1, 0)); // Y
        emitAxis(wp, wp + vec3(0, 0, s), vec3(0, 0, 1)); // Z
    } else {
        float pSize = 0.05; 
        
        gIsLine = 1.0;
        emitAxis(wp - vec3(pSize, 0, 0), wp + vec3(pSize, 0, 0), vec3(1.0));
        emitAxis(wp - vec3(0, pSize, 0), wp + vec3(0, pSize, 0), vec3(1.0));
    }
}