#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

class Torus {
private:
    float& R;
    float& r;
    int& sides;
    int segments = 0;

    void buildVertices() {
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

    void buildIndices() {
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

    float x(float u, float v) {
        return (R + r * std::cos(v)) * std::cos(u);
    }

    float y(float u, float v) {
        return (R + r * std::cos(v)) * std::sin(u);
    }

    float z(float u, float v) {
        return r * std::sin(v);
    }

    int idx(int i, int j) {
        return (i % segments) * sides + (j % sides);
    }

public:
    std::vector<float> vertices;
    std::vector<int> indices;

    Torus(float& R, float& r, int& sides) : R(R), r(r), sides(sides) {}

    void build(bool shouldRebuildIndices)
    {
        segments = sides * 2;

        buildVertices();
        if (shouldRebuildIndices) buildIndices();
    }
};