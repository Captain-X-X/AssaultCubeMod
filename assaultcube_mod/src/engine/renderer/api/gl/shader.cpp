#include "pch.h"
#include "shader.h"
#include "glutil.h"
#include "tools/filestream.h"

int Shader::getUniform(const std::string& name)
{
    if(m_uniformLocations.find(name) != m_uniformLocations.end()) {
        return m_uniformLocations[name];
    }
    unsigned int location = glGetUniformLocation(m_id, name.c_str());
    m_uniformLocations[name] = glGetUniformLocation(m_id, name.c_str());
    return location;
}

Shader::Shader()
    : m_id(NULL)
{
    m_file.clear();
}

Shader::~Shader()
{
    destroy();
}

bool Shader::load(const std::string& vertFile, const std::string& fragFile)
{
    m_file = vertFile + "," + fragFile;
    
    const char* vSrc = NULL;
    std::string vertSourceString = ReadFile(vertFile.c_str());
    if (!vertSourceString.empty()) {
        vSrc = vertSourceString.c_str();
    }
    
    const char* fSrc = NULL;
    std::string fragSourceString = ReadFile(fragFile.c_str());
    if (!fragSourceString.empty()) {
        fSrc = fragSourceString.c_str();
    }

    m_id = GL_CompileShader(vSrc, fSrc);
    if(m_id == NULL) {
        printf("failed to compile shaders.\n");
        return false;
    }

    printf("Shader created: %s [id=%d]\n", m_file.c_str(), m_id);
    return true;
}

bool Shader::load(const std::string& file)
{
    m_file = file;
    std::ifstream mfile(file.c_str());
    if (!mfile.is_open()) {
        printf("Shader: failed to open shader file: %s\n", m_file.c_str());
        return false;
    }

    enum class ShaderType_t { NONE, VERTEX, FRAGMENT };
    ShaderType_t currentType = ShaderType_t::NONE;
    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;
    std::string line;
    while (std::getline(mfile, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                currentType = ShaderType_t::VERTEX;
            } 
            else if (line.find("fragment") != std::string::npos) {
                currentType = ShaderType_t::FRAGMENT;
            } 
            else {
                currentType = ShaderType_t::NONE;
            }
            continue;
        } 
        switch (currentType) {
        case ShaderType_t::VERTEX: { vertexShaderStream << line << "\n"; break; }
        case ShaderType_t::FRAGMENT: { fragmentShaderStream << line << "\n"; break; }
        default:{ break; }
        }
    }

    m_id = GL_CompileShader(vertexShaderStream.str().c_str(), fragmentShaderStream.str().c_str());
    if(m_id == NULL){
        printf("failed to compile shaders. read info_log for more information!\n");
        return false;
    }

    printf("Shader created: %s [id=%d]\n", m_file.c_str(), m_id);
    return true;
}

void Shader::destroy()
{
    if(m_id != NULL) {
        glDeleteProgram(m_id);
        m_id = 0;
    }
}

void Shader::bind()
{
    glUseProgram(m_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(getUniform(name), value);
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(getUniform(name), value);
}

void Shader::setVec2(const std::string& name, const vec2_t& value)
{
    glUniform2f(getUniform(name), value.x, value.y);
}

void Shader::setVec3(const std::string& name, const vec3_t& value)
{
    glUniform3f(getUniform(name), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, const vec4_t& value)
{
    glUniform4f(getUniform(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat3(const std::string& name, const mat3_t& value)
{
    glUniformMatrix3fv(getUniform(name), 1, false, (float*)&value.elements);
}

void Shader::setMat4(const std::string& name, const mat4_t& value)
{
    glUniformMatrix4fv(getUniform(name), 1, false, (float*)&value.v);
}

// getters
unsigned int Shader::getId() const
{
    return m_id;
}
