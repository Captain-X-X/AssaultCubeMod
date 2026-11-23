#ifndef MATERIAL_H
#define MATERIAL_H
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "maths/vector_maths.h"
#include "../texture.h"
#include "../api/gl/Shader.h"

/*
* ==========
* 
* ==========
*/
typedef unsigned int materialHandle;
#define INVALID_MATERIAL 0

/*
* ==========
* main material
* ==========
*/
class Material
{
protected:
    std::string m_name;
    int m_id; // set by handler 

public:
    Texture* pTexture0;
    Vector4 colour;
    Shader* m_pShader;

public:
    Material();
    Material(const std::string& name, int id = -1);
    virtual ~Material();

    void setName(const std::string& name);
    void setTexture0(Texture* tex0);
    void setColour(const Vector4& col);
    void setShader(Shader* pShader);

    // getters
    bool hasTexture() const;
    bool isValid() const;
    const std::string& getName() const;
    const int getid() const; // returns -1 if there is not a valid id
    Shader* getShader() { return m_pShader; }
};

/*
* ==========
* material handler
* ==========
*/
class MaterialManager
{
private:
    std::vector<std::shared_ptr<Material>> m_materials;
    std::unordered_map<std::string, size_t> m_materialNameIndexEditor;
    Shader* m_pShaderDefault;

public:
    MaterialManager();
    ~MaterialManager();

    // clear and deallocate vector memory
    void shutdown();
    void createDefault();

    // add & return pointer to valid 
    Material* addMaterial(const Material& material);
    Material* getMaterial(const std::string& name);
    // Material* getMaterial(int id);
};
extern MaterialManager g_MaterialMgr;

#endif