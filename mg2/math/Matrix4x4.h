#pragma once
#include <array>
#include <cmath>
#include <iomanip>
#include "Vector4.h"

class Matrix4x4 {
public:
    std::array<std::array<float, 4>, 4> m;

    Matrix4x4(std::array<std::array<float, 4> , 4> m) : m(m) { }

    Matrix4x4() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = 0.0f;
    }

    static Matrix4x4 Identity() {
        std::array<std::array<float, 4>, 4> m;
        
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
        
        return Matrix4x4(m);
    }

    float* operator[](int row) { return m[row].data(); }
    const float* operator[](int row) const { return m[row].data(); }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result = Matrix4x4();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++)
                    result[i][j] += m[i][k] * other[k][j];
            }
        }
        return result;
    }

    // Mnożenie macierzy przez wektor 
    Vector4 operator*(const Vector4& v) const {
        float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
        float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
        float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

        return Vector4(x, y, z, w);
    }

    Vector4 multiplyRowVector(const Vector4& v) const {
        return Vector4(
            v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0],
            v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1],
            v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2],
            v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3]
        );
    }

    // Macierz translacji
    static Matrix4x4 translate(float tx, float ty, float tz) {
        Matrix4x4 result = Identity();
        result[0][3] = tx;
        result[1][3] = ty;
        result[2][3] = tz;
        return result;
    }

    static Matrix4x4 translate(Vector3 translateVector) {
        return translate(translateVector.x, translateVector.y, translateVector.z);
    }

    // Macierz skalowania
    static Matrix4x4 scale(float sx, float sy, float sz) {
        Matrix4x4 result = Identity();
        result[0][0] = sx;
        result[1][1] = sy;
        result[2][2] = sz;
        return result;
    }

    static Matrix4x4 scale(Vector3 scaleVector) {
        return scale(scaleVector.x, scaleVector.y, scaleVector.z);
    }

    // Macierz obrotu wokół osi X
    static Matrix4x4 rotateX(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[1][1] = c;  result[1][2] = -s;
        result[2][1] = s;  result[2][2] = c;
        return result;
    }


    // Macierz obrotu wokół osi Y
    static Matrix4x4 rotateY(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[0][0] = c;  result[0][2] = s;
        result[2][0] = -s; result[2][2] = c;
        return result;
    }

    // Macierz obrotu wokół osi Z
    static Matrix4x4 rotateZ(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[0][0] = c;  result[0][1] = -s;
        result[1][0] = s;  result[1][1] = c;
        return result;
    }

    static Matrix4x4 rotate(Vector3 rotationVector) {
        return rotateX(rotationVector.x) 
            * rotateY(rotationVector.y) 
            * rotateZ(rotationVector.z);
    }
    
    static Matrix4x4 projection(float aspect, float f, float n, float fovY_rad) {
        Matrix4x4 result;
        float t = std::tan(fovY_rad / 2.0f);   

        result[0][0] = 1.0f / (t * aspect);
        result[1][1] = 1.0f / t;
        result[2][2] = (f + n) / (f - n);
        result[2][3] = (-2.0f * f * n) / (f - n);
        result[3][2] = 1.0f;
        return result;
    }

    static Matrix4x4 projectionInverse(const Matrix4x4& P) {
        float a = P[0][0], b = P[1][1];
        float c = P[2][2], d = P[2][3];

        Matrix4x4 inv;
        inv[0][0] = 1.0f / a;
        inv[1][1] = 1.0f / b;
        inv[2][3] = 1.0f;           
        inv[3][2] = 1.0f / d;
        inv[3][3] = -c / d;
        return inv;
    }

    Matrix4x4 Transposed() const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[j][i] = m[i][j];
            }
        }
        return result;
    }

    std::array<float, 16> toFloatArray() const {
        std::array<float, 16> flat;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                flat[r * 4 + c] = m[r][c];
        return flat;
    }

    
    void print() const {
        for (int i = 0; i < 4; i++) {
            std::cout << "[ ";
            for (int j = 0; j < 4; j++) {
                std::cout << std::setw(8) << m[i][j];
                if (j < 3) std::cout << ", ";
            }
            std::cout << " ]\n";
        }
    }
};