#pragma once
#include <iostream>
#include "math/Matrix4x4.h"

class Transform {
public:
    Vector3& translation;
    Vector3& rotation;
    Vector3& scale;

    Transform(Vector3& translation, Vector3& rotation, Vector3& scale)
        : translation(translation), rotation(rotation), scale(scale) {}
};

// aktualny stan transformacji torusa
class Scene {
public:
    Transform& transform;
    Scene(Transform& transform) : transform(transform) {}

    Matrix4x4 modelMatrix() const {
        return Matrix4x4::translate(transform.translation)
            * Matrix4x4::rotate(transform.rotation)
            * Matrix4x4::scale(transform.scale);
    }

    Matrix4x4 modelMatrixInverse() const {
        return Matrix4x4::scale(1 / transform.scale.x, 1 / transform.scale.y, 1 / transform.scale.z)
            * Matrix4x4::rotateZ(transform.rotation.z).Transposed()
            * Matrix4x4::rotateY(transform.rotation.y).Transposed()
            * Matrix4x4::rotateX(transform.rotation.x).Transposed()
            * Matrix4x4::translate(-transform.translation);
    }

    Matrix4x4 view() const {
        Vector3 cameraPosition = Vector3(0, 0, 0.1);
        Vector3 cameraTarget = Vector3(0, 0, 0);
        Vector3 cameraDirection = (cameraPosition - cameraTarget).normalize();
        Vector3 up = Vector3(0, 1, 0);
        Vector3 cameraRight = up.cross(cameraDirection).normalize();
        Vector3 cameraUp = cameraDirection.cross(cameraRight);

        Matrix4x4 lookAt = Matrix4x4::Identity();

        lookAt[0][0] = cameraRight.x;
        lookAt[0][1] = cameraRight.y;
        lookAt[0][2] = cameraRight.z;
        lookAt[0][3] = -cameraRight.dot(cameraPosition);

        lookAt[1][0] = cameraUp.x;
        lookAt[1][1] = cameraUp.y;
        lookAt[1][2] = cameraUp.z;
        lookAt[1][3] = -cameraUp.dot(cameraPosition);

        lookAt[2][0] = cameraDirection.x;
        lookAt[2][1] = cameraDirection.y;
        lookAt[2][2] = cameraDirection.z;
        lookAt[2][3] = -cameraDirection.dot(cameraPosition);

        return lookAt;  
    }

    Matrix4x4 viewInverse() const {
        return Matrix4x4::Identity();
    }
};