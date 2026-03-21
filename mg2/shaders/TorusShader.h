#pragma once

#include "Shader.h"
#include "math/Matrix4x4.h";
#include "Nameof.h";
#include "../Torus.h";

class TorusShader : public Shader {
private:
    GLint uP, uM, uV;
    Matrix4x4& P;
    Matrix4x4& M;
    Matrix4x4& V;
    GLuint VAO, VBO, EBO;
    Torus& torus;
public:
    TorusShader(const std::string& vertexPath,
        const std::string& fragmentPath,
        Matrix4x4& P, Matrix4x4& M, Matrix4x4& V,
        Torus& torus)
        : P(P), M(M), V(V), torus(torus), Shader(vertexPath, fragmentPath)
    {
        uP = getUniform(nameof(uP));
        uM = getUniform(nameof(uM));
        uV = getUniform(nameof(uV));

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    void Rebuild(bool shouldRebuildIndices) {
        torus.build(shouldRebuildIndices);

        glBindVertexArray(VAO);

        // dane wierzchołków
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            torus.vertices.size() * sizeof(float),
            torus.vertices.data(), 
            GL_DYNAMIC_DRAW
        );

        glVertexAttribPointer(
            0,  // layout(location = 0) aPos;
            3, 
            GL_FLOAT, 
            GL_FALSE,
            3 * sizeof(float), (void*)0
        );

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            torus.indices.size() * sizeof(unsigned int),
            torus.indices.data(), GL_DYNAMIC_DRAW);
    }

    void Update() override {
        glUseProgram(program);
        glUniformMatrix4fv(uP, 1, GL_TRUE, P.toFloatArray().data());
        glUniformMatrix4fv(uM, 1, GL_TRUE, M.toFloatArray().data());
        glUniformMatrix4fv(uV, 1, GL_TRUE, V.toFloatArray().data());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, torus.indices.size(), GL_UNSIGNED_INT, 0);
    }
};