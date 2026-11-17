#ifndef MATERIAL_H
#define MATERIAL_H
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "texture.h"
#include "maths/vector_maths.h"

/*
* ==========
* pre def
* ==========
*/
// struct Vector4;
// class Texture;

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

public:
    Material();
    Material(const std::string& name, int id = -1);
    virtual ~Material();

    void setName(const std::string& name);
    void setTexture0(Texture* tex0);
    void setColour(const Vector4& col);

    // getters
    bool isValid() const;
    const std::string& getName() const;
    const int getid() const; // returns -1 if there is not a valid id
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

public:
    MaterialManager();
    ~MaterialManager();

    // clear and deallocate vector memory
    void shutdown();

    // add & return pointer to valid 
    Material* addMaterial(const Material& material);
    Material* getMaterial(const std::string& name);
    // Material* getMaterial(int id);
};

#endif