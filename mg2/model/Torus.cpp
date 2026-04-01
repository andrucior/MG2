#include "Torus.h"

void Torus::build(bool shouldRebuildIndices)
{
    segments = sides * 2;

    buildVertices();
    if (shouldRebuildIndices) buildIndices();
}

std::string Torus::toString() {
    return name;
}

std::string Torus::toHashCode() {
    return std::to_string(id);
}

void Torus::buildVertices() {
    vertices.clear();

    for (int i = 0; i < segments; i++) {
        // u - obrót głównej osi
        // segment - "plaster"
        float u = float(i) / segments * 2.0f * M_PI;

        for (int j = 0; j < sides; j++) {
            // punkty na obwodzie plastra
            float v = float(j) / sides * 2.0f * M_PI;

            vertices.push_back(x(u, v));
            vertices.push_back(y(u, v));
            vertices.push_back(z(u, v));
        }
    }
}

void Torus::buildIndices() {
    indices.clear();
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < sides; j++) {
            // dwa trójkąty 
            indices.push_back(idx(i, j));
            indices.push_back(idx(i + 1, j));
            indices.push_back(idx(i + 1, j + 1));

            indices.push_back(idx(i, j));
            indices.push_back(idx(i + 1, j + 1));
            indices.push_back(idx(i, j + 1));
        }
    }
}

float Torus::x(float u, float v) const {
    return (R + r * std::cos(v)) * std::cos(u);
}

float Torus::y(float u, float v) const {
    return (R + r * std::cos(v)) * std::sin(u);
}

float Torus::z(float u, float v) {
    return r * std::sin(v);
}

int Torus::idx(int i, int j) {
    return (i % segments) * sides + (j % sides);
}

bool Torus::baseNameTaken() {
   return takenNames.contains(
       nameof(Torus) + std::string(" ") + std::to_string(nextId)
   );
}
