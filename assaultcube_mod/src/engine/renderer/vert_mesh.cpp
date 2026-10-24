#include "cube.h"
#include "vert_mesh.h"
#include "vert_model.h"
#include "maths/maths_func.h"
#include "world_light.h"
#include "shadow.h"

extern int dynshadowsize;
extern int aadynshadow;
extern int saveshadows;
extern int dynshadowquad;
extern int shadowyaw;
extern vec3_t shadowdir;
extern vec3_t shadowpos;
extern int dbgstenc;
extern int dbgvlight;
extern int mdldlist;
extern vec3_t modelpos;
extern float modelyaw;
extern float modelpitch;
extern int animationinterpolationtime;

mesh::mesh() : name(0), owner(0), verts(0), tcverts(0), bbs(0), shareverts(0), tris(0), skin(notexture), tex(0), dynidx(0), dyndraws(0), statlist(0)
{
}

mesh::~mesh()
{
    DELETEA(name);
    DELETEA(verts);
    DELETEA(tcverts);
    DELETEA(bbs);
    DELETEA(shareverts);
    DELETEA(tris);
    if(statlist) glDeleteLists(statlist, 1);
    DELETEA(dynidx);
    DELETEA(dyndraws);
}

void mesh::genstrips()
{
    tristrip ts;
    ts.addtriangles(tris, numtris);
    vector<ushort> idxs;
    vector<drawcall> draws;
    ts.buildstrips(idxs, draws);
    dynidx = new ushort[idxs.length()];
    memcpy(dynidx, idxs.getbuf(), idxs.length()*sizeof(ushort));
    dynlen = idxs.length();
    dyndraws = new drawcall[draws.length()];
    memcpy(dyndraws, draws.getbuf(), draws.length()*sizeof(drawcall));
    numdyndraws = draws.length();
}

dyncacheentry* mesh::gendynverts(animstate &as, anpos &cur, anpos *prev, float ai_t)
{
    dyncacheentry *d = dyncache.start();
    int cachelen = 0;
    for(; d != dyncache.end(); d = d->next, cachelen++)
    {
        if(d->cur != cur) continue;
        if(prev)
        {
            if(d->prev == *prev && d->t == ai_t) return d;
        }
        else if(d->prev.fr1 < 0) return d;
    }

    d = vertmodel::dynalloc.allocate<dyncacheentry>((numverts + 1)*sizeof(vec3_t));
    if(!d) return NULL;

    if(cachelen >= owner->model->cachelimit) dyncache.removelast();
    dyncache.addfirst(d);
    vec3_t *buf = d->verts(),
        *vert1 = &verts[cur.fr1 * numverts],
        *vert2 = &verts[cur.fr2 * numverts],
        *pvert1 = NULL, *pvert2 = NULL;
    d->cur = cur;
    d->t = ai_t;
    if(prev)
    {
        d->prev = *prev;
        pvert1 = &verts[prev->fr1 * numverts];
        pvert2 = &verts[prev->fr2 * numverts];
    }
    else d->prev.fr1 = -1;

    #define iploop(body) \
        loopi(numverts) \
        { \
            vec3_t &v = buf[i]; \
            body; \
        }
    #define ip(p1, p2, t) (p1+t*(p2-p1))
    #define ip_v(p, c, t) ip(p##1[i].c, p##2[i].c, t)
    #define ip_v_ai(c) ip(ip_v(pvert, c, prev->t), ip_v(vert, c, cur.t), ai_t)
    if(prev) iploop(v = vec3_t(ip_v_ai(x), ip_v_ai(y), ip_v_ai(z)))
    else iploop(v = vec3_t(ip_v(vert, x, cur.t), ip_v(vert, y, cur.t), ip_v(vert, z, cur.t)))
    #undef ip
    #undef ip_v
    #undef ip_v_ai

    if(d->verts() == vertmodel::lastvertexarray) vertmodel::lastvertexarray = (void *)-1;

    return d;
}

void mesh::weldverts()
{
    hashtable<vec3_t, int> idxs;
    shareverts = new ushort[numverts];
    loopi(numverts) shareverts[i] = (ushort)idxs.access(verts[i], i);
    for(int i = 1; i < owner->numframes; i++)
    {
        const vec3_t *frame = &verts[i*numverts];
        loopj(numverts)
        {
            if(frame[j] != frame[shareverts[j]]) shareverts[j] = (ushort)j;
        }
    }
}

void mesh::findneighbors()
{
    hashtable<uint, uint> edges;
    loopi(numtris)
    {
        const tri &t = tris[i];
        loopj(3)
        {
            uint e1 = shareverts[t.vert[j]], e2 = shareverts[t.vert[(j+1)%3]], shift = 0;
            if(e1 > e2) { swap(e1, e2); shift = 16; }
            uint &edge = edges.access(e1 | (e2<<16), ~0U);
            if(((edge>>shift)&0xFFFF) != 0xFFFF) edge = 0;
            else
            {
                edge &= 0xFFFF<<(16-shift);
                edge |= i<<shift;
            }
        }
    }
    loopi(numtris)
    {
        tri &t = tris[i];
        loopj(3)
        {
            uint e1 = shareverts[t.vert[j]], e2 = shareverts[t.vert[(j+1)%3]], shift = 0;
            if(e1 > e2) { swap(e1, e2); shift = 16; }
            uint edge = edges[e1 | (e2<<16)];
            if(!edge || int((edge>>shift)&0xFFFF)!=i) t.neighbor[j] = 0xFFFF;
            else t.neighbor[j] = (edge>>(16-shift))&0xFFFF;
        }
    }
}

void mesh::cleanup()
{
    if(statlist)
    {
        glDeleteLists(1, statlist);
        statlist = 0;
    }
}

shadowcacheentry* mesh::genshadowvolume(animstate &as, anpos &cur, anpos *prev, float ai_t, vec3_t *buf)
{
    if(!shareverts) return NULL;

    shadowcacheentry *d = shadowcache.start();
    int cachelen = 0;
    for(; d != shadowcache.end(); d = d->next, cachelen++)
    {
        if(d->dir != shadowpos || d->cur != cur) continue;
        if(prev)
        {
            if(d->prev == *prev && d->t == ai_t) return d;
        }
        else if(d->prev.fr1 < 0) return d;
    }

    d = (owner->numframes > 1 || as.anim&ANIM_DYNALLOC ? vertmodel::dynalloc : vertmodel::statalloc).allocate<shadowcacheentry>(9*numtris*sizeof(ushort));
    if(!d) return NULL;

    if(cachelen >= owner->model->cachelimit) shadowcache.removelast();
    shadowcache.addfirst(d);
    d->dir = shadowpos;
    d->cur = cur;
    d->t = ai_t;
    if(prev) d->prev = *prev;
    else d->prev.fr1 = -1;

    static vector<uchar> side;
    side.setsize(0);

    loopi(numtris)
    {
        const tri &t = tris[i];
        const vec3_t &a = buf[t.vert[0]], &b = buf[t.vert[1]], &c = buf[t.vert[2]];
        side.add(vec3_t::cross((b - a), (c - a)).dot(shadowdir) <= 0 ? 1 : 0);
    }

    ushort *idx = d->idxs();
    loopi(numtris) if(side[i])
    {
        const tri &t = tris[i];
        loopj(3)
        {
            ushort n = t.neighbor[j];
            if(n==0xFFFF || !side[n])
            {
                ushort e1 = shareverts[t.vert[j]], e2 = shareverts[t.vert[(j+1)%3]];
                *idx++ = e2;
                *idx++ = e1;
                *idx++ = numverts;
            }
        }
    }

    if(dbgstenc >= (owner->numframes > 1 || as.anim&ANIM_DYNALLOC ? 2 : 1)) conoutf("%s: %d tris", owner->filename, (idx - d->idxs())/3);

    d->size = (uchar *)idx - (uchar *)d;
    return d;
}

lightcacheentry* mesh::lightvertexes(animstate &as, anpos &cur, anpos *prev, float ai_t, vec3_t *buf)
{
    if(dbgvlight) return NULL;

    lightcacheentry *d = lightcache.start();
    int cachelen = 0;
    for(; d != lightcache.end(); d = d->next, cachelen++)
    {
        if(d->lastcalclight != lastcalclight || d->pos != modelpos || d->yaw != modelyaw || d->pitch != modelpitch || d->cur != cur) continue;
        if(prev)
        {
            if(d->prev == *prev && d->t == ai_t) return d;
        }
        else if(d->prev.fr1 < 0) return d;
    }

    aabb_s curbb;
    getcurbb(curbb, as, cur, prev, ai_t);
    float dist = Maths::max(curbb.min.magnitude(), curbb.max.magnitude());
    if(OUTBORDRAD(modelpos.x, modelpos.y, dist)) return NULL;

    d = (owner->numframes > 1 || as.anim&ANIM_DYNALLOC ? vertmodel::dynalloc : vertmodel::statalloc).allocate<lightcacheentry>(numverts*sizeof(lightvert));
    if(!d) return NULL;

    if(cachelen >= owner->model->cachelimit) lightcache.removelast();
    lightcache.addfirst(d);
    d->lastcalclight = lastcalclight;
    d->pos = modelpos;
    d->yaw = modelyaw;
    d->pitch = modelpitch;
    d->cur = cur;
    d->t = ai_t;
    if(prev) d->prev = *prev;
    else d->prev.fr1 = -1;

    const mat4_t& m = vertmodel::matrixstack[vertmodel::matrixpos];
    lightvert *v = d->verts();
    loopi(numverts)
    {
        int x = (int)m.transformx(*buf), y = (int)m.transformy(*buf);
        const sqr *s = S(x, y);
        v->r = s->r;
        v->g = s->g;
        v->b = s->b;
        v->a = 255;
        v++;
        buf++;
    }
    if(d->verts() == vertmodel::lastcolorarray) vertmodel::lastcolorarray = (void *)-1;
    return d;
}

void mesh::getcurbb(aabb_s &b, animstate &as, anpos &cur, anpos *prev, float ai_t)
{
    b = bbs[cur.fr1];
    b.add(bbs[cur.fr2]);

    if(prev)
    {
        b.add(bbs[prev->fr1]);
        b.add(bbs[prev->fr2]);
    }
}

void mesh::render(animstate &as, anpos &cur, anpos *prev, float ai_t)
{
    if(!(as.anim&ANIM_NOSKIN))
    {
        GLuint id = tex < 0 ? -tex : skin->id;
        if(tex > 0) id = lookuptexture(tex)->id;
        if(id != vertmodel::lasttex)
        {
            glBindTexture(GL_TEXTURE_2D, id);
            vertmodel::lasttex = id;
        }
        if(vertmodel::enablealphablend) { glDisable(GL_BLEND); vertmodel::enablealphablend = false; }
        if(skin->bpp == 32 && owner->model->alphatest > 0)
        {
            if(!vertmodel::enablealphatest) { glEnable(GL_ALPHA_TEST); vertmodel::enablealphatest = true; }
            if(vertmodel::lastalphatest != owner->model->alphatest)
            {
                glAlphaFunc(GL_GREATER, owner->model->alphatest);
                vertmodel::lastalphatest = owner->model->alphatest;
            }
        }
        else if(vertmodel::enablealphatest) { glDisable(GL_ALPHA_TEST); vertmodel::enablealphatest = false; }
        if(!vertmodel::enabledepthmask) { glDepthMask(GL_TRUE); vertmodel::enabledepthmask = true; }
    }

    if(vertmodel::enableoffset)
    {
        disablepolygonoffset(GL_POLYGON_OFFSET_FILL);
        vertmodel::enableoffset = false;
    }

    bool isstat = as.frame==0 && as.range==1;
    if(isstat && statlist && !stenciling)
    {
        glCallList(statlist);
        xtraverts += statlen;
        return;
    }
    else if(stenciling==1)
    {
        aabb_s curbb;
        getcurbb(curbb, as, cur, prev, ai_t);
        glmatrixf mat;
        mat.mul(mvpmatrix, vertmodel::matrixstack[vertmodel::matrixpos]);
        if(!addshadowbox(curbb.min, curbb.max, shadowpos, mat)) return;
    }

    vec3_t *buf = verts;
    dyncacheentry *d = NULL;
    if(!isstat)
    {
        d = gendynverts(as, cur, prev, ai_t);
        if(!d) return;
        buf = d->verts();
    }
    if(vertmodel::lastvertexarray != buf)
    {
        if(!vertmodel::lastvertexarray) glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(vec3_t), buf);
        vertmodel::lastvertexarray = buf;
    }
    lightvert *vlight = NULL;
    if(as.anim&ANIM_NOSKIN && (!isstat || stenciling))
    {
        if(vertmodel::lasttexcoordarray)
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            vertmodel::lasttexcoordarray = NULL;
        }
    }
    else
    {
        if(vertmodel::lasttexcoordarray != tcverts)
        {
            if(!vertmodel::lasttexcoordarray) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, sizeof(tcvert), tcverts);
            vertmodel::lasttexcoordarray = tcverts;
        }
        if(owner->model->vertexlight)
        {
            if(d) d->locked = true;
            lightcacheentry *l = lightvertexes(as, cur, isstat ? NULL : prev, ai_t, buf);
            if(d) d->locked = false;
            if(l) vlight = l->verts();
        }
    }
    if(vertmodel::lastcolorarray != vlight)
    {
        if(vlight)
        {
            if(!vertmodel::lastcolorarray) glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(lightvert), vlight);
        }
        else glDisableClientState(GL_COLOR_ARRAY);
        vertmodel::lastcolorarray = vlight;
    }
    if(stenciling)
    {
        if(d) d->locked = true;
        shadowcacheentry *s = genshadowvolume(as, cur, isstat ? NULL : prev, ai_t, buf);
        if(d) d->locked = false;
        if(!s) return;
        buf[numverts] = s->dir;
        glDrawElements(GL_TRIANGLES, s->numidxs(), GL_UNSIGNED_SHORT, s->idxs());
        xtraverts += s->numidxs();
        return;
    }

    bool builddlist = isstat && !owner->model->vertexlight && mdldlist;
    if(builddlist) glNewList(statlist = glGenLists(1), GL_COMPILE);
    loopi(numdyndraws)
    {
        const drawcall &d = dyndraws[i];
        if(hasDRE && !builddlist) glDrawRangeElements_(d.type, d.minvert, d.maxvert, d.count, GL_UNSIGNED_SHORT, &dynidx[d.start]);
        else glDrawElements(d.type, d.count, GL_UNSIGNED_SHORT, &dynidx[d.start]);
    }
    if(builddlist)
    {
        glEndList();
        glCallList(statlist);
        statlen = dynlen;
    }
    xtraverts += dynlen;
}

int mesh::findvert(int axis, int dir)
{
    if(axis<0 || axis>2) return -1;
    int vert = -1;
    float bestval = -1e16f;
    loopi(numverts)
    {
        float val = verts[i][axis]*dir;
        if(val > bestval)
        {
            vert = i;
            bestval = val;
        }
    }
    return vert;
}

float mesh::calcradius()
{
    float rad = 0;
    loopi(numverts) rad = Maths::max(rad, verts[i].magnitudexy());
    return rad;
}

void mesh::calcneighbors()
{
    if(!shareverts)
    {
        weldverts();
        findneighbors();
    }
}

void mesh::calcbbs()
{
    if(bbs) return;
    bbs = new aabb_s[owner->numframes];
    loopi(owner->numframes)
    {
        aabb_s &b = bbs[i];
        b.min = vec3_t(1e16f, 1e16f, 1e16f);
        b.max = vec3_t(-1e16f, -1e16f, -1e16f);
        const vec3_t *frame = &verts[numverts*i];
        loopj(numverts) b.add(frame[j]);
    }
}
