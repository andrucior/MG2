#version 330 core

in vec3  gColor;
in float gSelected;
in float gHighlighted;
in float gIsLine;     

out vec4 fragColor;

void main() {
    vec3 baseColor;
    if (gHighlighted > 0.5) baseColor = vec3(0.8, 0.4, 0.0);
    else if (gSelected > 0.5) baseColor = vec3(1.0, 1.0, 0.0);
    else baseColor = gColor;

    fragColor = vec4(baseColor, 1.0);
}