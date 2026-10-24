#ifndef TRISTRIP_H
#define TRISTRIP_H
#include "tools/cube_types.h"
#include "tools/cube_vector.h"

extern const int dbgts;

struct drawcall {
    GLenum type;
    GLuint start, minvert, maxvert;
    GLsizei count;

    drawcall();
    drawcall(GLenum type, GLuint start, GLsizei count = 0);
};

struct triangle {
    enum {
        // must be larger than all other triangle/vert indices
        UNUSED = 0xFFFE,
        REMOVED = 0xFFFF
    };
    ushort v[3], n[3];

    bool link(ushort neighbor, ushort old = UNUSED);
    void unlink(ushort neighbor, ushort unused = UNUSED);
    int numlinks() const;
    bool hasvert(ushort idx) const;
    ushort diffvert(ushort v1, ushort v2);
    bool haslink(ushort neighbor) const;
};

struct edge {
    ushort from,
        to;
};

struct tristrip {
    vector<triangle> triangles;
    vector<ushort> connectivity[4];
    vector<uchar> nodes;

    template <class T>
    void addtriangles(const T *tris, int numtris)
    {
        if (dbgts)
            printf("addtriangles: tris = %d, inds = %d", numtris, numtris * 3);
        for (int i = 0; i < numtris; i++) {
            triangle &tri = triangles.add();
            for (int j = 0; j < 3; j++) {
                tri.v[j] = tris[i].vert[j];
                tri.n[j] = triangle::UNUSED;
            }
            if (tri.v[0] == tri.v[1] || tri.v[1] == tri.v[2] || tri.v[2] == tri.v[0]) {
                if (dbgts)
                    printf("degenerate triangle");
                triangles.drop();
            }
        }
    }

    void findconnectivity();
    void removeconnectivity(ushort i);
    bool remaining() const;
    ushort leastconnected();
    int findedge(const triangle &from, const triangle &to, ushort v = triangle::UNUSED);
    void removenodes(int i);
    ushort nexttriangle(triangle &tri, bool &nextswap, ushort v1 = triangle::UNUSED, ushort v2 = triangle::UNUSED);
    void buildstrip(vector<ushort> &strip, bool reverse = false, bool prims = false);
    void buildstrips(vector<ushort> &strips, vector<drawcall> &draws, bool prims = true, bool degen = false);
};

uint hthash(const edge &x);
bool htcmp(const edge &x, const edge &y);

#endif;