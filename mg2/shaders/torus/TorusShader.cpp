#include "TorusShader.h"

TorusShader::TorusShader(Matrix4x4& P, Camera& camera, Torus& torus)
    : P(P), torus(torus), camera(camera), Shader(vertexPath, fragmentPath) {
    uP = getUniform(nameof(uP));
    uM = getUniform(nameof(uM));
    uV = getUniform(nameof(uV));
    uIsSelected = getUniform(nameof(uIsSelected));
    uIsHighlighted = getUniform(nameof(uIsHighlighted));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void TorusShader::Rebuild(bool shouldRebuildIndices) {
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

void TorusShader::Update() {
    glUseProgram(program);
    glUniformMatrix4fv(uP, 1, GL_TRUE, P.toFloatArray().data());
    glUniformMatrix4fv(uM, 1, GL_TRUE, torus.modelMatrix().toFloatArray().data());
    glUniformMatrix4fv(uV, 1, GL_TRUE, camera.view().toFloatArray().data());
    glUniform1i(uIsSelected, torus.isSelected);
    glUniform1i(uIsHighlighted, torus.isHighlighted);
    
}

void TorusShader::draw() {
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, torus.indices.size(), GL_UNSIGNED_INT, 0);
}
