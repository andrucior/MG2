#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include <algorithm>
#include <set>
#include "TransformableObject.h"
#include "SceneObject.h"

class Torus : public TransformableObject {
private:
    int id;
    static inline int nextId = 0;
    static inline std::set<std::string> takenNames;

    void buildVertices();
    void buildIndices();
    float x(float u, float v) const;
    float y(float u, float v) const;
    float z(float u, float v);
    int idx(int i, int j);
    bool baseNameTaken();

public:
    float R;
    float r;
    int sides;
    int segments = 0;
    std::vector<float> vertices;
    std::vector<int> indices;

    Torus(float R, float r, int sides, Transform transform = Transform())
        : R(R), r(r), id(nextId++), sides(sides), TransformableObject(transform) {

        initName(std::string(nameof(Torus) + std::string(" ") + std::to_string(id)));
    }

    void build(bool shouldRebuildIndices);

    std::string toString() override;

    std::string toHashCode() override;
};