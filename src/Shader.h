#pragma once
#include "config.h"

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setInt(const std::string &name, int value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setFloat(const std::string &name, float value) const;
};
