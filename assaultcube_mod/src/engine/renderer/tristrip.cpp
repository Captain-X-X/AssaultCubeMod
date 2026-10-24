#include "cube.h"
#include "tristrip.h"

const int dbgts = 0;
VAR(tsswap, 0, 1, 1);

//
drawcall::drawcall() 
{

}

drawcall::drawcall(GLenum type, GLuint start, GLsizei count) 
    : type(type), start(start), minvert(~0U), maxvert(0), count(count) 
{

}

// triangle
bool triangle::link(ushort neighbor, ushort old)
{
    loopi(3)
    {
        if (n[i] == neighbor)
            return true;
        else if (n[i] == old)
        {
            n[i] = neighbor;
            return true;
        }
    }
    if (dbgts && old == UNUSED)
        conoutf("excessive links");
    return false;
}

void triangle::unlink(ushort neighbor, ushort unused)
{
    loopi(3) if (n[i] == neighbor) n[i] = unused;
}

int triangle::numlinks() const
{
    int num = 0;
    loopi(3) if (n[i] < UNUSED) num++;
    return num;
}

bool triangle::hasvert(ushort idx) const
{
    loopi(3) if (v[i] == idx) return true;
    return false;
}

ushort triangle::diffvert(ushort v1, ushort v2)
{
    loopi(3) if (v[i] != v1 && v[i] != v2) return v[i];
    return UNUSED;
}

bool triangle::haslink(ushort neighbor) const
{
    loopi(3) if (n[i] == neighbor) return true;
    return false;
}

/*
* ==========
* tristrip
* ==========
*/
void tristrip::findconnectivity()
{
    hashtable<edge, ushort> edges;
    nodes.setsize(0);
    loopv(triangles)
    {
        triangle &tri = triangles[i];
        loopj(3)
        {
            edge e = {tri.v[j == 2 ? 0 : j + 1], tri.v[j]};
            edges.access(e, i);

            while (nodes.length() <= tri.v[j])
                nodes.add(0);
            nodes[tri.v[j]]++;
        }
    }
    loopv(triangles)
    {
        triangle &tri = triangles[i];
        loopj(3)
        {
            edge e = {tri.v[j], tri.v[j == 2 ? 0 : j + 1]};
            ushort *owner = edges.access(e);
            if (!owner)
                continue;
            else if (!tri.link(*owner))
            {
                if (dbgts)
                    conoutf("failed linkage 1: %d -> %d", *owner, i);
            }
            else if (!triangles[*owner].link(i))
            {
                if (dbgts)
                    conoutf("failed linkage 2: %d -> %d", *owner, i);
                tri.unlink(*owner);
            }
        }
    }
    loopi(4) connectivity[i].setsize(0);
    loopv(triangles) connectivity[triangles[i].numlinks()].add(i);
    if (dbgts)
        conoutf("no connections: %d", connectivity[0].length());
}

void tristrip::removeconnectivity(ushort i)
{
    triangle &tri = triangles[i];
    loopj(3) if (tri.n[j] < triangle::UNUSED)
    {
        triangle &neighbor = triangles[tri.n[j]];
        int conn = neighbor.numlinks();
        bool linked = false;
        loopk(3) if (neighbor.n[k] == i)
        {
            linked = true;
            neighbor.n[k] = triangle::REMOVED;
            break;
        }
        if (linked)
        {
            connectivity[conn].replacewithlast(tri.n[j]);
            connectivity[conn - 1].add(tri.n[j]);
        }
    }
    removenodes(i);
}

bool tristrip::remaining() const
{
    loopi(4) if (!connectivity[i].empty()) return true;
    return false;
}

ushort tristrip::leastconnected()
{
    loopi(4) if (!connectivity[i].empty())
    {
        ushort least = connectivity[i].pop();
        removeconnectivity(least);
        return least;
    }
    return triangle::UNUSED;
}

int tristrip::findedge(const triangle &from, const triangle &to, ushort v)
{
    loopi(3) loopj(3)
    {
        if (from.v[i] == to.v[j] && from.v[i] != v)
            return i;
    }
    return -1;
}

void tristrip::removenodes(int i)
{
    loopj(3) nodes[triangles[i].v[j]]--;
}

ushort tristrip::nexttriangle(triangle &tri, bool &nextswap, ushort v1, ushort v2)
{
    ushort next = triangle::UNUSED;
    int nextscore = 777;
    nextswap = false;
    loopi(3) if (tri.n[i] < triangle::UNUSED)
    {
        triangle &nexttri = triangles[tri.n[i]];
        int score = nexttri.numlinks();
        bool swap = false;
        if (v1 != triangle::UNUSED)
        {
            if (!nexttri.hasvert(v1))
            {
                swap = true;
                score += nodes[v2] > nodes[v1] ? 1 : -1;
            }
            else if (nexttri.hasvert(v2))
                continue;
            else
                score += nodes[v1] > nodes[v2] ? 1 : -1;
            if (!tsswap && swap)
                continue;
            score += swap ? 1 : -1;
        }
        if (score < nextscore)
        {
            next = tri.n[i];
            nextswap = swap;
            nextscore = score;
        }
    }
    if (next != triangle::UNUSED)
    {
        tri.unlink(next, triangle::REMOVED);
        connectivity[triangles[next].numlinks()].replacewithlast(next);
        removeconnectivity(next);
    }
    return next;
}

void tristrip::buildstrip(vector<ushort> &strip, bool reverse, bool prims)
{
    ushort prev = leastconnected();
    if (prev == triangle::UNUSED)
        return;
    triangle &first = triangles[prev];
    bool doswap;
    ushort cur = nexttriangle(first, doswap);
    if (cur == triangle::UNUSED)
    {
        loopi(3) strip.add(first.v[!prims && reverse && i >= 1 ? 3 - i : i]);
        return;
    }
    int from = findedge(first, triangles[cur]),
        to = findedge(first, triangles[cur], first.v[from]);
    if (from + 1 != to)
        swap(from, to);
    strip.add(first.v[(to + 1) % 3]);
    if (reverse)
        swap(from, to);
    strip.add(first.v[from]);
    strip.add(first.v[to]);

    ushort v1 = first.v[to], v2 = first.v[from];
    while (cur != triangle::UNUSED)
    {
        prev = cur;
        cur = nexttriangle(triangles[prev], doswap, v1, v2);
        if (doswap)
            strip.add(v2);
        ushort v = triangles[prev].diffvert(v1, v2);
        strip.add(v);
        if (!doswap)
            v2 = v1;
        v1 = v;
    }
}

void tristrip::buildstrips(vector<ushort> &strips, vector<drawcall> &draws, bool prims, bool degen)
{
    vector<ushort> singles;
    findconnectivity();
    int numtris = 0, numstrips = 0;
    while (remaining())
    {
        vector<ushort> strip;
        bool reverse = degen && !strips.empty() && (strips.length() & 1);
        buildstrip(strip, reverse, prims);
        numstrips++;
        numtris += strip.length() - 2;
        if (strip.length() == 3 && prims)
        {
            loopv(strip) singles.add(strip[i]);
            continue;
        }

        if (!strips.empty() && degen)
        {
            strips.dup();
            strips.add(strip[0]);
        }
        else
            draws.add(drawcall(GL_TRIANGLE_STRIP, strips.length()));
        drawcall &d = draws.last();
        loopv(strip)
        {
            ushort index = strip[i];
            strips.add(index);
            d.minvert = Maths::min(d.minvert, (GLuint)index);
            d.maxvert = Maths::max(d.maxvert, (GLuint)index);
        }
        d.count = strips.length() - d.start;
    }
    if (prims && !singles.empty())
    {
        drawcall &d = draws.add(drawcall(GL_TRIANGLES, strips.length()));
        loopv(singles)
        {
            ushort index = singles[i];
            strips.add(index);
            d.minvert = Maths::min(d.minvert, (GLuint)index);
            d.maxvert = Maths::max(d.maxvert, (GLuint)index);
        }
        d.count = strips.length() - d.start;
    }
    if (dbgts)
        conoutf("strips = %d, tris = %d, inds = %d, merges = %d", numstrips, numtris, numtris + numstrips * 2, (degen ? 2 : 1) * (numstrips - 1));
}

//
uint hthash(const edge &x) 
{ 
    return x.from ^ x.to; 
}

bool htcmp(const edge &x, const edge &y) 
{ 
    return x.from == y.from && x.to == y.to; 
}
