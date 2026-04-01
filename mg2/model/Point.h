#pragma once

#include <string>

#include "Vector3.h"
#include "Nameof.h"
#include "SceneObject.h"

class Point : public SceneObject {
private:
    int id;
    bool baseNameTaken();
public:
    Vector3 position;
    static inline int nextId = 0;
    bool isCenterPoint = false;

    Point(Vector3 position = Vector3(), bool isCenterPoint = false)
        : position(position), id(nextId++), isCenterPoint(isCenterPoint) {
        
        initName(std::string(nameof(Point) + std::string(" ") + std::to_string(id)));  
    }

    Matrix4x4 modelMatrix() const override;

    Matrix4x4 modelMatrixInverse() const override;

    std::string toString() override;

    std::string toHashCode() override;  

    void rotateAroundPoint(float ax, float ay, float az, const Vector3& pivot);

    void scaleAroundPoint(float sx, float sy, float sz, const Vector3& pivot);

    void transformAroundPoint(float ax, float ay, float az,
        float sx, float sy, float sz,
        const Vector3& pivot);
};