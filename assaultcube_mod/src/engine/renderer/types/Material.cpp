#include "pch.h"
#include "Material.h"
#include "../api/gl/Shader.h"

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

void Material::setShader(Shader* pShader)
{
    m_pShader = pShader;
}

// getters
bool Material::hasTexture() const
{
    if (pTexture0 != nullptr) {
        return true;
    }
    return false;
}

bool Material::isValid() const
{
    if (m_name.empty() == false) {
        return true;
    }
    return false;
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

    if (m_pShaderDefault != nullptr) {
        delete m_pShaderDefault;
        m_pShaderDefault = nullptr;
    }
}

void MaterialManager::createDefault()
{
    if (m_pShaderDefault == nullptr) {
        m_pShaderDefault = new Shader();
        m_pShaderDefault->load("bin_unix/data/shaders/gl_default2d.vert", "bin_unix/data/shaders/gl_default2d.frag");
    }

    // quake IOQ3
    constexpr int width = 8;
    constexpr int height = 8;
    unsigned char whiteImage[width * height * 4];
    memset(whiteImage, 255, sizeof(whiteImage));

    Texture tex0;
    tex0.createImage("white_image", (unsigned char*)&whiteImage, 8, 8, GL_RGBA);
    
    g_TextureMgr.add(tex0, false);

    Material mat;
    mat.setName("white");
    mat.setColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mat.pTexture0 = g_TextureMgr.getByFileName("white_image");
    mat.setShader(m_pShaderDefault);
    addMaterial(mat);
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
            if (m_materials[i]->getName() == name) {
                return m_materials[i].get();
            }
        }
    }
    return nullptr;
}

MaterialManager g_MaterialMgr;