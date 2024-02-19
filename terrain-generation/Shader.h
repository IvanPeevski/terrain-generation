#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void deAllocate();
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif
