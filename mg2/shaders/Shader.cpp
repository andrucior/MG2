#include "Shader.h"

std::string Shader::loadShaderSource(const std::string& filePath) {
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

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
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

Shader::Shader(
    const std::string vertexCodePath,
    const std::string fragmentCodePath,
    const std::optional<std::string>& geomCodePath
) {
    vertexCode = loadShaderSource(vertexCodePath);
    fragmentCode = loadShaderSource(fragmentCodePath);

    vsId = compileShader(GL_VERTEX_SHADER, vertexCode);
    fsId = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    program = glCreateProgram();
    glAttachShader(program, vsId);
    glAttachShader(program, fsId);

    if (geomCodePath.has_value()) {
        geomCode = loadShaderSource(geomCodePath.value());
        gsId = compileShader(GL_GEOMETRY_SHADER, geomCode);
        glAttachShader(program, gsId);
    }

    glLinkProgram(program);
}