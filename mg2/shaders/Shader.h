#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <optional>

#include "SceneObject.h"
#include "Matrix4x4.h"

class Shader {
private:
	std::string vertexCode, fragmentCode, geomCode;
    
    std::string loadShaderSource(const std::string& filePath);

    unsigned int compileShader(unsigned int type, const std::string& source);

public:
    unsigned int vsId, fsId, gsId, program;

    Shader(
        const std::string vertexCodePath, 
        const std::string fragmentCodePath,
        const std::optional<std::string>& geomCodePath = std::nullopt
    );

    virtual void Update() = 0;

    GLint getUniform(const char* name) {
        return glGetUniformLocation(program, name);
    }

    void setUniformMatrix4x4(GLint id, Matrix4x4 data) {
        return glUniformMatrix4fv(id, 1, GL_TRUE, data.toFloatArray().data());
    }

    void setUniform1f(GLint id, float value) {
        glUniform1f(id, 2.0f);
    }

    virtual bool ownsObject(const SceneObject*) const { return false; }

};