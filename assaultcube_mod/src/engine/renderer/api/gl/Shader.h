#ifndef GLSHADER_H
#define GLSHADER_H

#include "pch.h"

class Shader
{
private:
    std::string m_file;
    unsigned int m_id;
    std::unordered_map<std::string, int> m_uniformLocations;

    int getUniform(const std::string& name);

public:
    Shader();
    ~Shader();

    bool load(const std::string& vertFile, const std::string& fragFile);
    bool load(const std::string& file);
    void destroy();

    void bind();
    void unbind();

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const vec2_t& value);
    void setVec3(const std::string& name, const vec3_t& value);
    void setVec4(const std::string& name, const vec4_t& value);
    void setMat3(const std::string& name, const mat3_t& value);
    void setMat4(const std::string& name, const mat4_t& value);

    // getters
    unsigned int getId() const;
};

#endif