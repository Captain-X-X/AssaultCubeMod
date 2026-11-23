#ifndef RENDERERSPRITE_H
#define RENDERERSPRITE_H

#include "../types/Mesh2D.h"
#include "../types/Material.h"

class Sprite
{
private:
    Mesh2D m_mesh;
    Material* m_pMaterial;
    Vector4 m_colour;
    bool m_created;

public:
    Sprite();
    virtual ~Sprite();

    void create(float x, float y, float width, float height, const Vector4& colour);
    void create(float x, float y, float width, float height, const Vector4& tint, Material* pMaterial);
    void update(float x, float y, float width, float height);

    // getters
    bool isCreated() const { return m_created; } 
    Mesh2D& getMesh() { return m_mesh; }
    Material* getMaterial() { return m_pMaterial; }
    const Vector4& getTint() const;
};

#endif