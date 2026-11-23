/*
* ==========
* this is 2D ONLY... the 3d mesh is just called Mesh...
* ==========
*/
#ifndef RENDERERMESH2D_H
#define RENDERERMESH2D_H

#include <vector>
#include "VertexData.h"

class Mesh2D
{
private:
    std::vector<VertexData2D> m_vertices;
    std::vector<unsigned int> m_indices;

public:
    Mesh2D();
    virtual ~Mesh2D();

    void allocMeshSize(size_t verts, size_t indis);
    void mergeMesh(Mesh2D& mesh);
    void destroyMesh(); // swap with empty vectors
    void reset(); // .clear

    void addVertex(const VertexData2D& vert);
    void addIndex(unsigned int index);

    // getters
    std::vector<VertexData2D>& getVertices()    { return m_vertices; };
    std::vector<unsigned int>& getIndices()     { return m_indices; };
};

#endif