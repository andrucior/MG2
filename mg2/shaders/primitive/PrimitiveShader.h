#pragma once

#include <vector>
#include <memory>

#include "Shader.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Point.h"

class PrimitiveShader : public Shader {
private:
    static inline std::string vsPath = "./shaders/primitive/vs_primitive.glsl",
        fsPath = "./shaders/primitive/fs_primitive.glsl",
        gsPath = "./shaders/primitive/gs_primitive.glsl";
    std::vector<float> verts;
    GLuint VAO, VBO;
    GLint uP, uV, uM, uColor, uAxisSize;
    Matrix4x4& P;
    Camera& camera;
public:
    PrimitiveShader(Matrix4x4& P, Camera& camera);

    void Rebuild(
        const std::vector<std::shared_ptr<SceneObject>>& objects,
        std::shared_ptr<Point> centerPoint,
        std::shared_ptr<Point> cursor
    );

    void Update() override;
};