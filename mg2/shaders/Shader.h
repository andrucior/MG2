#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>


class Shader {
private:
	std::string vertexCode, fragmentCode;
    
    std::string loadShaderSource(const std::string& filePath) {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            shaderFile.open(filePath);
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            return shaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::COULD NOT READ FILE: " << filePath << std::endl;
            return "";
        }
    }

    unsigned int compileShader(unsigned int type, const std::string& source) {
        unsigned int s = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);

        int success;
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(s, 512, NULL, infoLog);
            std::cerr << "ERROR::COMPILATION FAILED:\n" << infoLog << std::endl;
        }
        return s;
    }

public:
    unsigned int vsId, fsId, program;

    Shader(const std::string vertexCodePath, const std::string fragmentCodePath) {
        vertexCode = loadShaderSource(vertexCodePath);
        fragmentCode = loadShaderSource(fragmentCodePath);

        vsId = compileShader(GL_VERTEX_SHADER, vertexCode);
        fsId = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

        program = glCreateProgram();
        glAttachShader(program, vsId);
        glAttachShader(program, fsId);
        glLinkProgram(program);
    }

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

};