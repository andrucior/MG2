#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include "Matrix4x4.h"
#include "Nameof.h"
#include "Transform.h"
#include "SceneObject.h"

class TransformableObject : public SceneObject {
public:
    TransformableObject(Transform transform = Transform()) : transform(transform) {}

    Transform transform;

    virtual std::string toString() = 0;
    virtual std::string toHashCode() = 0;

    Matrix4x4 modelMatrix() const override {
        return Matrix4x4::translate(transform.translation)
            * Matrix4x4::rotate(transform.rotation)
            * Matrix4x4::scale(transform.scale);
    }

    Matrix4x4 modelMatrixInverse() const override {
        return Matrix4x4::scale(1.0f / transform.scale.x, 1.0f / transform.scale.y, 1.0f / transform.scale.z)
            * Matrix4x4::rotateZ(transform.rotation.z).Transposed()
            * Matrix4x4::rotateY(transform.rotation.y).Transposed()
            * Matrix4x4::rotateX(transform.rotation.x).Transposed()
            * Matrix4x4::translate(-transform.translation);
    }

    void rotateAroundPoint(float ax, float ay, float az, const Vector3& pivot) {
        Matrix4x4 T = Matrix4x4::translate(pivot);
        Matrix4x4 Ti = Matrix4x4::translate(-pivot);
        Matrix4x4 R = Matrix4x4::rotateX(ax) * Matrix4x4::rotateY(ay) * Matrix4x4::rotateZ(az);

        Matrix4x4 newM = T * R * Ti * Matrix4x4::translate(transform.translation);
        transform.translation = Vector3(newM[0][3], newM[1][3], newM[2][3]);

        transform.rotation.x = wrapAngle(transform.rotation.x + ax);
        transform.rotation.y = wrapAngle(transform.rotation.y + ay);
        transform.rotation.z = wrapAngle(transform.rotation.z + az);
    }

    void scaleAroundPoint(float sx, float sy, float sz, const Vector3& pivot) {
        transform.translation.x = pivot.x + sx * (transform.translation.x - pivot.x);
        transform.translation.y = pivot.y + sy * (transform.translation.y - pivot.y);
        transform.translation.z = pivot.z + sz * (transform.translation.z - pivot.z);

        transform.scale.x *= sx;
        transform.scale.y *= sy;
        transform.scale.z *= sz;
    }

    void transformAroundPoint(float ax, float ay, float az,
        float sx, float sy, float sz,
        const Vector3& pivot)
    {
        if (ax != 0.0f || ay != 0.0f || az != 0.0f)
            rotateAroundPoint(ax, ay, az, pivot);
        if (sx != 1.0f || sy != 1.0f || sz != 1.0f)
            scaleAroundPoint(sx, sy, sz, pivot);
    }

private:
    static float wrapAngle(float angle) {
        return fmod(fmod(angle, 2.0f * (float)M_PI) + 2.0f * (float)M_PI, 2.0f * (float)M_PI);
    }
};