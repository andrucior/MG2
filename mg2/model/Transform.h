#pragma once
#include "../math/Vector3.h"

class Transform {
public:
    Vector3 translation;
    Vector3 rotation;
    Vector3 scale;

    Transform() {
        translation = Vector3(0, 0, 0);
        rotation = Vector3(0, 0, 0);
        scale = Vector3(1, 1, 1);
    }

    Transform(Vector3 translation, Vector3 rotation, Vector3 scale)
        : translation(translation), rotation(rotation), scale(scale) {}
};
