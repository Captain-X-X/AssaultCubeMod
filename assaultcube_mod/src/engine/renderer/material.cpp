#include "material.h"

Material::Material()
    : m_id(-1), pTexture0(nullptr), colour(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
    m_name.clear();
}

Material::Material(const std::string& name, int id) 
    : m_name(name), m_id(id), pTexture0(nullptr), colour(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
    if (name.empty()) {
        //m_name.clear();
    }
}

Material::~Material()
{
    // if (pTexture0) {
    //     if (pTexture0->id != 0) {
            
    //     }
    // }
}

void Material::setName(const std::string& name)
{
    if (name.empty()) {
        printf("material: failed to set name- is invalid/empty\n");
        return;
    }
    this->m_name = name;
}

void Material::setTexture0(Texture* tex0)
{
    if (!tex0) {
        return;
    }
    this->pTexture0 = tex0;
}

void Material::setColour(const Vector4& col)
{
    this->colour = col;
}

// getters
bool Material::isValid() const
{
    if (m_name.empty()) {
        printf("invalid material!!!");
        return false;
    }

    // if (pTexture0 == nullptr) {
    //     return false;
    // }
    return true;
}

const std::string& Material::getName() const
{
    return m_name;
}

const int Material::getid() const
{
    return m_id;
}

/*
* ==========
* material handler
* ==========
*/
MaterialManager::MaterialManager()
{
    // m_materials.reserve(2);
}

MaterialManager::~MaterialManager()
{

}

// this needs some more thinking...
void MaterialManager::shutdown()
{
    //
    if (m_materialNameIndexEditor.empty() == false) {
        m_materialNameIndexEditor.clear();
    }

    if (m_materials.empty() == false) {
        m_materials.clear();
        m_materials.shrink_to_fit();
    }
}

Material* MaterialManager::addMaterial(const Material& material)
{
    
    if (material.isValid()) {
        // avoid copys
        Material* pMat = getMaterial(material.getName());
        if (pMat != nullptr) {
            return pMat;
        }

        for (size_t i = 0; i < m_materials.size(); i++) {
            if (m_materials[i] == nullptr) {
                m_materials[i] = std::make_shared<Material>(material);
                return m_materials[i].get();
            }
        }

        m_materials.push_back(std::make_shared<Material>(material));
        return m_materials.back().get();
    }
    return nullptr;
}

Material* MaterialManager::getMaterial(const std::string& name)
{
    for (size_t i = 0; i < m_materials.size(); i++) {
        if (m_materials[i] != nullptr) {
            return m_materials[i].get();
        }
    }
    return nullptr;
}
