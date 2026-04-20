#pragma once

#include "Shader.h"
#include "Nameof.h"
#include "Camera.h"
#include "Torus.h"

class TorusShader : public Shader {
private:
    static inline const std::string vertexPath = "./shaders/torus/vs_torus.glsl";
    static inline const std::string fragmentPath = "./shaders/torus/fs_torus.glsl";

    GLint uP, uM, uV, uIsSelected, uIsHighlighted;
    Matrix4x4& P;
    Camera& camera;
    GLuint VAO, VBO, EBO;
public:
    Torus& torus;

    TorusShader(
        Matrix4x4& P, Camera& camera, Torus& torus);

    void Rebuild(bool shouldRebuildIndices);

    bool ownsObject(const SceneObject* obj) const override {
        return &torus == obj;
    }

    void draw() override;
    void Update() override;
};