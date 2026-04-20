#include "Point.h"

Matrix4x4 Point::modelMatrix() const {
    return Matrix4x4::translate(position);
}

Matrix4x4 Point::modelMatrixInverse() const {
    return Matrix4x4::translate(-position);
}

std::string Point::toString() {
    return name;
}

std::string Point::toHashCode() {
    size_t h1 = std::hash<std::string>{}(nameof(Point));
    size_t h2 = std::hash<int>{}(id);
    return std::to_string(h1 ^ (h2 << 1));
}

void Point::rotateAroundPoint(float ax, float ay, float az, const Vector3& pivot) {
    Matrix4x4 T = Matrix4x4::translate(pivot);
    Matrix4x4 Ti = Matrix4x4::translate(-pivot);
    Matrix4x4 R = Matrix4x4::rotateX(ax) * Matrix4x4::rotateY(ay) * Matrix4x4::rotateZ(az);

    Matrix4x4 newM = T * R * Ti * Matrix4x4::translate(position);
    position = Vector3(newM[0][3], newM[1][3], newM[2][3]);
}

void Point::scaleAroundPoint(float sx, float sy, float sz, const Vector3& pivot) {
    position.x = pivot.x + sx * (position.x - pivot.x);
    position.y = pivot.y + sy * (position.y - pivot.y);
    position.z = pivot.z + sz * (position.z - pivot.z);
}

void Point::transformAroundPoint(float ax, float ay, float az,
    float sx, float sy, float sz,
    const Vector3& pivot)
{
    if (ax != 0.0f || ay != 0.0f || az != 0.0f)
        rotateAroundPoint(ax, ay, az, pivot);
    if (sx != 1.0f || sy != 1.0f || sz != 1.0f)
        scaleAroundPoint(sx, sy, sz, pivot);
}