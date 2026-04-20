#version 330 core

out vec4 fragColor;
uniform bool uIsSelected;
uniform bool uIsHighlighted;

void main() {
    if (uIsHighlighted) {
        fragColor = vec4(0.8, 0.4, 0.0, 1.0);
    } else if (uIsSelected) {
        fragColor = vec4(1.0, 1.0, 0.0, 0.6);
    } else {
        fragColor = vec4(0.5, 0.5, 1.0, 1.0);
    }
}