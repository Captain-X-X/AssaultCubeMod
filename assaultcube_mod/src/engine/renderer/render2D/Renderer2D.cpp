#include "pch.h"
#include "Renderer2D.h"

void PrintMatrix4(Matrix4& mat)
{
    Vector4 vCamArry[4];
    // 0, 1, 2, 3
    memcpy(&vCamArry[0].x, &mat.v[0], (sizeof(float) * 4));
    
    // 4, 5, 6, 7
    memcpy(&vCamArry[1].x, &mat.v[4], (sizeof(float) * 4));
    
    // 8, 9, 10, 11
    memcpy(&vCamArry[2].x, &mat.v[8], (sizeof(float) * 4));
    
    // 12, 13, 14, 15
    memcpy(&vCamArry[3].x, &mat.v[12], (sizeof(float) * 4));
    printf("ortho:\n");
    printf(" vec4[0]: { %.9f, %.9f, %.9f, %.9f }\n", vCamArry[0].x, vCamArry[0].y, vCamArry[0].z, vCamArry[0].w);
    printf(" vec4[1]: { %.9f, %.9f, %.9f, %.9f }\n", vCamArry[1].x, vCamArry[1].y, vCamArry[1].z, vCamArry[1].w);
    printf(" vec4[2]: { %.9f, %.9f, %.9f, %.9f }\n", vCamArry[2].x, vCamArry[2].y, vCamArry[2].z, vCamArry[2].w);
    printf(" vec4[3]: { %.9f, %.9f, %.9f, %.9f }\n", vCamArry[3].x, vCamArry[3].y, vCamArry[3].z, vCamArry[3].w);
    printf("end:\n");
}

void Renderer2D::uploadVertexData(VertexData2D* map, RenderCommand2D& cmd)
{
    if (map == nullptr) {
        return;
    }

    if (m_vertCount == RENDERER2D_VBUFFER_SIZE) {
        printf("Renderer2D: max vertex count hit! [max=%ld]\n", RENDERER2D_VBUFFER_SIZE);
        return;
    }

    Mesh2D* pMesh = cmd.pMesh;
    VertexData2D* pMeshVertexData = pMesh->getVertices().data();
    for (size_t i = 0; i < pMesh->getVertices().size(); i++) {
        map->position.x = ( pMeshVertexData[i].position.x + cmd.transform.origin.x );
        map->position.y = ( pMeshVertexData[i].position.y + cmd.transform.origin.y );
        map->position.z = ( pMeshVertexData[i].position.z + cmd.transform.origin.z );

        map->uv.x = pMeshVertexData[i].uv.x;
        map->uv.y = pMeshVertexData[i].uv.y;

        if (cmd.bUseMaterialColour) {
            if (cmd.pMaterial) {
                map->colour.x = cmd.pMaterial->colour.x;
                map->colour.y = cmd.pMaterial->colour.y;
                map->colour.z = cmd.pMaterial->colour.z;
                map->colour.w = cmd.pMaterial->colour.w;
            }
        }
        else {
            map->colour.x = pMeshVertexData[i].colour.x;
            map->colour.y = pMeshVertexData[i].colour.y;
            map->colour.z = pMeshVertexData[i].colour.z;
            map->colour.w = pMeshVertexData[i].colour.w;
        }
        map++;
    }
    map += pMesh->getVertices().size();
    m_vertCount += pMesh->getVertices().size();
}

void Renderer2D::uploadIndexData(unsigned int* map, RenderCommand2D& cmd, unsigned int vertexBaseOffset)
{
    if (map == nullptr) {
        return;
    }
    if (m_indxCount == RENDERER2D_IBUFFER_SIZE) {
        printf("Renderer2D: max vertex count hit! [max=%d]\n", RENDERER2D_IBUFFER_SIZE);
        return;
    }

    Mesh2D* pMesh = cmd.pMesh;
    unsigned int meshISize = pMesh->getIndices().size();
    for (size_t i = 0; i < meshISize; i++) {
        unsigned int v = pMesh->getIndices()[i] + vertexBaseOffset;
        map[i] = v;
    }
    m_indxCount += pMesh->getIndices().size();
}

Renderer2D::Renderer2D()
    : m_pVertexBuffer(nullptr), m_pVertexArray(nullptr), m_pindexBuffer(nullptr)
{
    m_vertCount = 0;
    m_indxCount = 0;
}

Renderer2D::~Renderer2D()
{
    shutdown();
}

bool Renderer2D::initialize(const Camera2D& camera)
{
    if (m_pVertexArray == nullptr) {
        m_pVertexArray = new VertexArray();
        m_pVertexArray->create();
    }
 
    if (m_pVertexBuffer == nullptr) {
        m_pVertexArray->bind();
        m_pVertexBuffer = new VertexBuffer();
        m_pVertexBuffer->create(RENDERER2D_VBUFFER_SIZE, VertexBufferLayoutType_t::DEF_2D);
    }

    if (m_pindexBuffer == nullptr) {
        m_pindexBuffer = new IndexBuffer();
        m_pindexBuffer->create(RENDERER2D_IBUFFER_SIZE);
    }

    m_pVertexBuffer->unbind();
    m_pVertexArray->unbind();
    m_pindexBuffer->unbind();

    m_camera = camera;
    m_viewport = m_camera.getViewport();

    return true;
}

void Renderer2D::shutdown()
{
    if (m_pVertexArray != nullptr) {
        m_pVertexArray->destroy();
        delete m_pVertexArray;
        m_pVertexArray = nullptr;
    }
 
    if (m_pVertexBuffer != nullptr) {
        m_pVertexBuffer->destroy();
        delete m_pVertexBuffer;
        m_pVertexBuffer = nullptr;
    }

    if (m_pindexBuffer != nullptr) {
        m_pindexBuffer->destroy();
        delete m_pindexBuffer;
        m_pindexBuffer = nullptr;
    }
}

void Renderer2D::begin()
{
    m_collectDrawCommands = true;
}

void Renderer2D::submitSprite(Sprite& sprite, const Transform& transform)
{
    if (sprite.isCreated()) {
        RenderCommand2D cmd = RenderCommand2D();
        cmd.type = RenderCommand2DTypes_t::DRAW_INDEXED;
        cmd.pMesh = &sprite.getMesh();
        cmd.pMaterial = sprite.getMaterial();
        cmd.transform = transform;
        cmd.bUseMaterialColour = false;
        m_cmds.push_back(cmd);
    }
}

void Renderer2D::submitSpriteStatic(Sprite& sprite, const Transform& transform)
{
    if (sprite.isCreated()) {
        m_sprites.push_back(sprite);
    }
}

void Renderer2D::end()
{
    if (m_collectDrawCommands == false) {
        return;
    }
    m_collectDrawCommands = false;
    if (m_cmds.empty()) {
        if (m_sprites.empty()) {
            return;
        }
    }

    for (size_t i = 0; i < m_sprites.size(); i++) {
        RenderCommand2D cmd = RenderCommand2D();
        cmd.type = RenderCommand2DTypes_t::DRAW_INDEXED;
        cmd.pMesh = &m_sprites[i].getMesh();
        cmd.pMaterial = m_sprites[i].getMaterial();
        
        cmd.transform = Transform();
        cmd.transform.origin.x = 0.0f;
        cmd.transform.origin.y = 0.0f;
        cmd.transform.origin.z = 0.0f;
        cmd.transform.rotation.x = 0.0f;
        cmd.transform.rotation.y = 0.0f;
        cmd.transform.rotation.z = 0.0f;

        cmd.bUseMaterialColour = false;
        m_cmds.push_back(cmd);
    }

    // std::sort(m_cmds.begin(), m_cmds.end(), [](const RenderCommand2D& a, const RenderCommand2D& b){
    //     std::string name0;
    //     std::string name1;
    //
    //     if (a.pMaterial) {
    //         name0 = a.pMaterial->getName();
    //     }
    //
    //     if (b.pMaterial) {
    //         name1 = b.pMaterial->getName();
    //     }
    //
    //     return name0 < name1;
    // });

    VertexData2D* pVertexBufferData = (VertexData2D*)m_pVertexBuffer->mapBuffer();
    unsigned int* pIndexBufferData = (unsigned int*)m_pindexBuffer->mapBuffer();

    unsigned int baseVertexOffset = 0;
    unsigned int baseIndexOffset = 0;
    for (size_t i = 0; i < m_cmds.size(); i++) {
        RenderCommand2D& cmd = m_cmds[i];

        uploadVertexData(pVertexBufferData, cmd);
        pVertexBufferData += cmd.pMesh->getVertices().size();

        switch (m_cmds[i].type) {
        case RenderCommand2DTypes_t::DRAW_ARRAYS: { break; }
        case RenderCommand2DTypes_t::DRAW_INDEXED: {
            uploadIndexData(pIndexBufferData, cmd, baseVertexOffset);
            baseIndexOffset += cmd.pMesh->getIndices().size();
            pIndexBufferData += baseIndexOffset;
            break;
        }
        default: { break; }
        }
        
        baseVertexOffset += cmd.pMesh->getVertices().size();
    }

    m_pVertexBuffer->unmapBuffer();
    m_pindexBuffer->unmapBuffer();
}

void Renderer2D::flush()
{
    glViewport(
        m_viewport.x,
        m_viewport.y,
        m_viewport.width,
        m_viewport.height
    );

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Material* pMat = g_MaterialMgr.getMaterial("white");
    Shader* pShader = nullptr;
    if (pMat) {
        pShader = pMat->getShader();
        pShader->bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pMat->pTexture0->getId());
        pShader->setInt("u_Texture0", 0);
        
        m_camera.update();
        pShader->setMat4("u_ViewMatrix", m_camera.getViewMatrix());
    }

    m_pVertexBuffer->bind();
    m_pVertexArray->bind();
    m_pindexBuffer->bind();

    // glDrawArrays(GL_TRIANGLES, 0, m_vertCount);
    glDrawElements(GL_TRIANGLES, m_indxCount, GL_UNSIGNED_INT, 0);
    m_pVertexArray->unbind();
    m_pindexBuffer->unbind();
    m_pVertexBuffer->unbind();
    pShader->unbind();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    m_indxCount = 0;
    m_vertCount = 0;
    m_cmds.clear();
    m_sprites.clear();
}

void Renderer2D::setViewport(const Viewport& vp)
{
    m_viewport = vp;
}

std::vector<RenderCommand2D>& Renderer2D::getRenderCommands()
{
    return m_cmds;
}

const std::vector<RenderCommand2D>& Renderer2D::getRenderCommands() const
{
    return m_cmds;
}