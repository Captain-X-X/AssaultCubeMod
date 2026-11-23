#include "pch.h"
#include "Mesh2D.h"

Mesh2D::Mesh2D()
{

}

Mesh2D::~Mesh2D()
{

}

void Mesh2D::allocMeshSize(size_t verts, size_t indis)
{
    m_vertices.resize(verts);
    m_indices.resize(indis);
}

void Mesh2D::mergeMesh(Mesh2D& mesh)
{
    std::vector<VertexData2D>& vertexData = mesh.getVertices();
    std::vector<unsigned int>& indexData = mesh.getIndices();

    if (vertexData.empty()) {
        printf("Mesh2D Merge: mesh has no vertices!\n");
    }
    else {
        m_vertices.insert(
            m_vertices.end(),
            vertexData.begin(),
            vertexData.end()
        );
    }

    size_t baseIndexOffset = m_indices.size();
    m_indices.reserve(baseIndexOffset + indexData.size());
    for (size_t i = 0; i < indexData.size(); i++) {
        m_indices.push_back(indexData[i] + baseIndexOffset);
    }
}

void Mesh2D::destroyMesh()
{
    std::vector<VertexData2D> vertexDataEmpty;
    std::vector<unsigned int> indexDataEmpty;
    m_vertices.swap(vertexDataEmpty);
    m_indices.swap(indexDataEmpty);
}

void Mesh2D::reset()
{
    m_vertices.clear();
    m_indices.clear();
}

void Mesh2D::addVertex(const VertexData2D& vert)
{
    m_vertices.push_back(vert);
}

void Mesh2D::addIndex(unsigned int index)
{
    m_indices.push_back(index);
}
