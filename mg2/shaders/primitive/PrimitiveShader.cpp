#include "PrimitiveShader.h"

PrimitiveShader::PrimitiveShader(Matrix4x4& P, Camera& camera)
    : P(P), camera(camera), Shader(vsPath, fsPath, gsPath) {
    
    uP = getUniform(nameof(uP));
    uV = getUniform(nameof(uV));
    uM = getUniform(nameof(uM));
    uAxisSize = getUniform(nameof(uAxisSize));
    uColor = getUniform(nameof(uColor));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void PrimitiveShader::Rebuild(const std::vector<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<Point> centerPoint, std::shared_ptr<Point> cursor)
{
    verts.clear();

    for (const auto& obj : objects) {
        auto point = std::dynamic_pointer_cast<Point>(obj);
        if (!point) continue;
        // Geometria
        verts.push_back(point->position.x);
        verts.push_back(point->position.y);
        verts.push_back(point->position.z);
        // Flagi 
        verts.push_back(point->isSelected ? 1.0f : 0.0f);
        verts.push_back(point->isHighlighted ? 1.0f : 0.0f);
        verts.push_back(0.0f);
        verts.push_back(0.0f);
    }
    if (centerPoint != nullptr) {
        // Geometria
        verts.push_back(centerPoint->position.x);
        verts.push_back(centerPoint->position.y);
        verts.push_back(centerPoint->position.z);
        // Flagi 
        verts.push_back(0.0f);
        verts.push_back(0.0f);
        verts.push_back(1.0f);
        verts.push_back(0.0f);
    }
    if (cursor != nullptr) {
        verts.push_back(cursor->position.x);
        verts.push_back(cursor->position.y);
        verts.push_back(cursor->position.z);
        // Flagi 
        verts.push_back(0.0f);
        verts.push_back(0.0f);
        verts.push_back(0.0f);
        verts.push_back(1.0f);
    }

    if (!verts.empty()) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

        // Atrybut 0: Pozycja (x, y, z)           
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Atrybut 1: Status (isSelected, isHighlighted, isCenterPoint, isCursor)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
}

void PrimitiveShader::Update() {
    if (verts.empty()) return;

    glUseProgram(program);
    glUniformMatrix4fv(uP, 1, GL_TRUE, P.toFloatArray().data());
    glUniformMatrix4fv(uV, 1, GL_TRUE, camera.view().toFloatArray().data());
    glUniformMatrix4fv(uM, 1, GL_TRUE, Matrix4x4::Identity().toFloatArray().data());
    glUniform1f(uAxisSize, 0.1f);
    glUniform3f(uColor, 1.f, 1.f, 1.f);
    glPointSize(6.f);
    
}

void PrimitiveShader::draw() {
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(verts.size() / 7));
}

