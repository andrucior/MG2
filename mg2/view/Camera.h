#pragma once

#include <cmath>

#include "TransformableObject.h"
#include "Vector2.h"

class Camera : public TransformableObject {
public:
    Vector3 cameraTarget = Vector3(0, 0, 0);
    Vector3 cameraPosition = Vector3(0.0f, 0.2f, 1.9f);
    
    Vector3 targetGoal = Vector3(0, 0, 0); 
    float   lerpSpeed = 5.0f;

    Camera() : TransformableObject() {
        name = nameof(Camera);
    }

    Vector3 screenToWorld(float screenX, float screenY, int screenW, int screenH,
        int panelWidth, const Matrix4x4& P, float targetY = 0.0f) const
    {
        float ndcX = (2.0f * (screenX - panelWidth) / (screenW - panelWidth)) - 1.0f;
        float ndcY = 1.0f - (2.0f * screenY / screenH);

        Matrix4x4 invP = Matrix4x4::projectionInverse(P);
        Vector3 rayView = Vector3((invP * Vector4(ndcX, ndcY, -1.0f, 1.0f)).xyz());
        rayView.z = 1.0f;
        Vector3 rayWorld = Vector3(
            (viewInverse() * Vector4(rayView.x, rayView.y, rayView.z, 0.0f)).xyz()
        ).normalize();

        if (std::abs(rayWorld.y) < 1e-6f)
            return cameraPosition + rayWorld * (cameraTarget - cameraPosition).length();

        float t = (targetY - cameraPosition.y) / rayWorld.y;
        if (t < 0) t = (cameraTarget - cameraPosition).length();
        return cameraPosition + rayWorld * t;
    }

    Vector2 worldToScreen(const Vector3& worldPos, const Matrix4x4& P,
        int viewX, int viewY, int viewW, int viewH, float& outDepth) const
    {
        Vector4 clip = P * view() * Vector4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
        if (clip.w == 0.0f) clip.w = 1e-6f;
        Vector3 ndc = Vector3(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);

        outDepth = clip.z / clip.w;

        float sx = (ndc.x * 0.5f + 0.5f) * viewW + viewX;
        float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewH;

        return Vector2(sx, sy);
    }

	Matrix4x4 view() const {
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
        Vector3 cameraDirection = (cameraPosition - cameraTarget).normalize();
        Vector3 up = Vector3(0, 1, 0);
        Vector3 cameraRight = up.cross(cameraDirection).normalize();
        Vector3 cameraUp = cameraDirection.cross(cameraRight);

        Matrix4x4 inv = Matrix4x4::Identity();

        inv[0][0] = cameraRight.x;
        inv[0][1] = cameraUp.x;
        inv[0][2] = cameraDirection.x;

        inv[1][0] = cameraRight.y;
        inv[1][1] = cameraUp.y;
        inv[1][2] = cameraDirection.y;

        inv[2][0] = cameraRight.z;
        inv[2][1] = cameraUp.z;
        inv[2][2] = cameraDirection.z;

        inv[0][3] = cameraPosition.x;
        inv[1][3] = cameraPosition.y;
        inv[2][3] = cameraPosition.z;

        return inv;
    }

    std::string toString() override {
        return nameof(Camera);
    }

    std::string toHashCode() override {
        size_t h1 = std::hash<float>{}(cameraPosition.x);
        size_t h2 = std::hash<float>{}(cameraPosition.y);
        size_t h3 = std::hash<float>{}(cameraPosition.z);
        size_t h4 = std::hash<std::string>{}(nameof(Camera));

        size_t combined = h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);

        return std::to_string(combined);
    }

    void update(float deltaTime) {
        // Lerp w stronę celu
        cameraTarget = Vector3::lerp(cameraTarget, targetGoal, lerpSpeed * deltaTime);
    }
};
