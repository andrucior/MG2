#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "math/Vector3.h"
#include "math/Matrix4x4.h"

class LightController {
private:
    Vector3 cameraPos = { 0,0, 30 };

    int width, height;

    // Gradient f(u) = u^T D u => grad = 2 D u
    Vector3 computeNormal(const Matrix4x4& Dp, const Vector4& p) const {
        Vector4 vec = Dp * p;
        return Vector3(vec.x, vec.y, vec.z).normalize();
    }

public:
    LightController(int width, int height)
        : width(width), height(height) {
    }

    float intensity(const Vector3& v, const Vector3& n, float m) const {
        float dot = std::max(0.0f, v.dot(n));
        return std::pow(dot, m);
    }

    void fillPixels(std::vector<unsigned char>& pixels) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int baseIdx = y * width + x;
                pixels[baseIdx * 3] = 0;
                pixels[baseIdx * 3 + 1] = 0;
                pixels[baseIdx * 3 + 2] = 0;
            }
        }
    }
};