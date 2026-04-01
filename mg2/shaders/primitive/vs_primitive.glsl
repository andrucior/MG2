#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aStatus; // x = selected, y = highlighted, z = centerPoint

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uM;

out float vSelected;
out float vHighlighted;
out float vCenterPoint;
out float vCursor;
out vec3 vWorldPos;

void main() {
    gl_Position = uP * uV * uM * vec4(aPos, 1.0);
    gl_PointSize = 6.0;
    
    vSelected = aStatus.x;
    vHighlighted = aStatus.y;
    vCenterPoint = aStatus.z;
    vCursor = aStatus.w;
    vWorldPos = vec3(uM * vec4(aPos, 1.0));
}