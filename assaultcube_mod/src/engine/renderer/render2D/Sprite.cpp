#include "pch.h"
#include "Sprite.h"

Sprite::Sprite()
    : m_created(false)
{
    if (m_mesh.getVertices().empty()) {
        m_mesh.allocMeshSize(4, 6);
    }
}

Sprite::~Sprite()
{

}

void Sprite::create(float x, float y, float width, float height, const Vector4& colour)
{
    m_mesh.reset();
    m_colour = colour;
    m_pMaterial = g_MaterialMgr.getMaterial("white");
    if (m_pMaterial == nullptr) {
        printf("failed to add default white material to sprite!\n");
    }
    // else {
    //     m_material.setTexture0(g_TextureMgr.getByFileName("white"));
    //     m_material.setName("sprite_coloured");
    // }

    VertexData2D v2d;

    // set sprite colour
    v2d.colour.x = colour.x;
    v2d.colour.y = colour.y;
    v2d.colour.z = colour.z;
    v2d.colour.w = colour.w;

    // this is 2d so hard set all Z axis to 0
    v2d.position.z = 0.0f;

    // top right
    v2d.position.x = x;
    v2d.position.y = y;
    v2d.uv.x = 0.0f;
    v2d.uv.y = 0.0f;
    m_mesh.addVertex(v2d);
    
    // bottom right
    v2d.position.x = x;
    v2d.position.y = y + height;
    v2d.uv.x = 0.0f;
    v2d.uv.y = 1.0f;
    m_mesh.addVertex(v2d);
    
    // bottom left
    v2d.position.x = x + width;
    v2d.position.y = y + height;
    v2d.uv.x = 1.0f;
    v2d.uv.y = 1.0f;
    m_mesh.addVertex(v2d);
    
    // top left
    v2d.position.x = x + width;
    v2d.position.y = y;
    v2d.uv.x = 1.0f;
    v2d.uv.y = 0.0f;
    m_mesh.addVertex(v2d);
    
    m_mesh.addIndex(0);
    m_mesh.addIndex(1);
    m_mesh.addIndex(2);
    m_mesh.addIndex(2);
    m_mesh.addIndex(3);
    m_mesh.addIndex(0);
    m_created = true;
}

void Sprite::create(float x, float y, float width, float height, const Vector4& tint, Material* pMaterial)
{
    m_mesh.reset();
    m_pMaterial = pMaterial;
    m_colour = tint;

    VertexData2D v2d;

    // set sprite colour
    v2d.colour.x = tint.x;
    v2d.colour.y = tint.y;
    v2d.colour.z = tint.z;
    v2d.colour.w = tint.w;

    // this is 2d so hard set all Z axis to 0
    v2d.position.z = 0.0f;

    // top right
    v2d.position.x = x;
    v2d.position.y = y;
    v2d.uv.x = 0.0f;
    v2d.uv.y = 0.0f;
    m_mesh.addVertex(v2d);
    
    // bottom right
    v2d.position.x = x;
    v2d.position.y = y + height;
    v2d.uv.x = 0.0f;
    v2d.uv.y = 1.0f;
    m_mesh.addVertex(v2d);
    
    // bottom left
    v2d.position.x = x + width;
    v2d.position.y = y + height;
    v2d.uv.x = 1.0f;
    v2d.uv.y = 1.0f;
    m_mesh.addVertex(v2d);
    
    // top left
    v2d.position.x = x + width;
    v2d.position.y = y;
    v2d.uv.x = 1.0f;
    v2d.uv.y = 0.0f;
    m_mesh.addVertex(v2d);
    
    m_mesh.addIndex(0);
    m_mesh.addIndex(1);
    m_mesh.addIndex(2);
    m_mesh.addIndex(2);
    m_mesh.addIndex(3);
    m_mesh.addIndex(0);
    m_created = true;
}

void Sprite::update(float x, float y, float width, float height)
{
    if (m_created == false) {
        return;
    }

    std::vector<VertexData2D>& vData = m_mesh.getVertices();

    if (vData.empty()) {
        return;
    }

    // 0
    vData[0].position.x = x;
    vData[0].position.y = y;
    vData[0].position.z = 0.0f;
    vData[0].uv.x = 0.0f;
    vData[0].uv.y = 0.0f;
    
    // 1
    vData[1].position.x = x;
    vData[1].position.y = y + height;
    vData[1].position.z = 0.0f;
    vData[1].uv.x = 0.0f;
    vData[1].uv.y = 1.0f;
    
    // 2
    vData[2].position.x = x + width;
    vData[2].position.y = y + height;
    vData[2].position.z = 0.0f;
    vData[2].uv.x = 1.0f;
    vData[2].uv.y = 1.0f;
    
    // 3
    vData[3].position.x = x + width;
    vData[3].position.y = y;
    vData[3].position.z = 0.0f;
    vData[3].uv.x = 1.0f;
    vData[3].uv.y = 0.0f;
}

const Vector4& Sprite::getTint() const
{
    return m_colour;
}