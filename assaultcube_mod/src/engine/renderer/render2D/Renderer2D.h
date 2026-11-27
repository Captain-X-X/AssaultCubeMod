/*
* ==========
* this is a version 1 and prob will change soon
* ==========
*/
#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "../api/gl/VertexBuffer.h"
#include "../api/gl/VertexArray.h"
#include "../api/gl/IndexBuffer.h"
#include "../api/gl/Shader.h"
#include "../types/Material.h"
#include "../types/Transform.h"
#include "../types/Mesh2D.h"
#include "../types/Camera2D.h"
#include "../types/Viewport.h"
#include "Sprite.h"

#define RENDERER2D_MAX_SPRITES  100000 // 100K  
#define RENDERER2D_VBUFFER_SIZE (sizeof(VertexData2D) * 4) * RENDERER2D_MAX_SPRITES
#define RENDERER2D_IBUFFER_SIZE (                       6) * RENDERER2D_MAX_SPRITES

/*
* ==========
* 
* ==========
*/
enum class RenderCommand2DTypes_t : unsigned int {
    DRAW_INDEXED,
    DRAW_ARRAYS,
    DRAW_INSTANCED
};

struct RenderCommand2D
{
    // draw type
    RenderCommand2DTypes_t  type;
    
    // render data
    Mesh2D* pMesh;
    Material* pMaterial;
    Transform transform;
    bool bUseMaterialColour;
};

/*
* ==========
* 
* ==========
*/
class Renderer2D
{
private:
    bool                            m_collectDrawCommands;
    std::vector<RenderCommand2D>    m_cmds;
    std::vector<Sprite>             m_sprites; // end turns these into cmds and clears this vector at the end
    Viewport                        m_viewport;
    Camera2D                        m_camera;
    // api data
    VertexBuffer*                   m_pVertexBuffer;
    VertexArray*                    m_pVertexArray;
    IndexBuffer*                    m_pindexBuffer;

    unsigned int m_vertCount;
    unsigned int m_indxCount;

    void uploadVertexData(VertexData2D* map, RenderCommand2D& cmd);
    void uploadIndexData(unsigned int* map, RenderCommand2D& cmd, unsigned int vertexBaseOffset);

public:
    Renderer2D();
    virtual ~Renderer2D();

    bool initialize(const Camera2D& camera);
    void shutdown();

    void begin();
    void submitSprite(Sprite& sprite, const Transform& transform);
    void submitSpriteStatic(Sprite& sprite, const Transform& transform);

    // theese start with a capital letter as im unsure if they will stay as methods.
    //  maybe there should be some type of renderer interface that handles immediate style rendering functions.
    void SubmitRectCMD(float x, float y, float w, float h, const Vector4& tint, Material* pmat);
    void SubmitRectRotatedCMD(float x, float y, float w, float h, float rot, const Vector4& tint, Material* pmat);

    void end();
    void flush();

    //
    void setViewport(const Viewport& vp);

    std::vector<RenderCommand2D>& getRenderCommands();
    const std::vector<RenderCommand2D>& getRenderCommands() const;
    Viewport& getViewport() { return m_viewport; }
};

#endif