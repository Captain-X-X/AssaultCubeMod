#include "cube.h"
#include "vert_model.h"
#include "vert_animation.h"
#include "render_particles.h"
#include "shadow.h"
#include "render_cubes.h"

VARP(dynshadowsize, 4, 5, 8);
VARP(aadynshadow, 0, 2, 3);
VARP(saveshadows, 0, 1, 1);
VARP(dynshadowquad, 0, 0, 1);
VAR(shadowyaw, 0, 45, 360);
vec3_t shadowdir(0, 0, -1), shadowpos(0, 0, 0);
int dbgstenc = 0;
int dbgvlight = 0;
VARP(mdldlist, 0, 1, 1);
vec3_t modelpos;
float modelyaw, modelpitch;
extern int animationinterpolationtime;

part::part() : filename(NULL), anims(NULL), links(NULL), tags(NULL), numtags(0), shadows(NULL), shadowrad(0) {}
part::~part()
{
    DELETEA(filename);
    meshes.deletecontents();
    DELETEA(anims);
    DELETEA(links);
    DELETEA(tags);
    if (shadows)
        glDeleteTextures(numframes, shadows);
    DELETEA(shadows);
}

void part::cleanup()
{
    loopv(meshes) meshes[i]->cleanup();
    if (shadows)
    {
        glDeleteTextures(numframes, shadows);
        DELETEA(shadows);
    }
}

bool part::link(part *link, const char *tag, vec3_t *pos)
{
    loopi(numtags) if (!strcmp(tags[i].name, tag))
    {
        links[i].p = link;
        links[i].pos = pos;
        return true;
    }
    return false;
}

bool part::gentag(const char *name, int *verts, int numverts, mesh *m)
{
    if (!m)
    {
        if (meshes.empty())
            return false;
        m = meshes[0];
    }
    if (numverts < 1)
        return false;
    loopi(numverts) if (verts[i] < 0 || verts[i] > m->numverts) return false;

    tag *ntags = new tag[(numtags + 1) * numframes];
    ntags[numtags].name = newstring(name);
    loopi(numframes)
    {
        memcpy(&ntags[(numtags + 1) * i], &tags[numtags * i], numtags * sizeof(tag));

        tag *t = &ntags[(numtags + 1) * i + numtags];
        t->pos = m->verts[m->numverts * i + verts[0]];
        if (numverts > 1)
        {
            for (int j = 1; j < numverts; j++)
                t->pos.add(m->verts[m->numverts * i + verts[j]]);
            t->pos.div(numverts);
        }
        t->identity();
    }
    loopi(numtags) tags[i].name = NULL;

    DELETEA(tags);
    tags = ntags;
    numtags++;

    linkedpart *nlinks = new linkedpart[numtags];
    loopi(numtags - 1) swap(links[i].emitter, nlinks[i].emitter);
    DELETEA(links);
    links = nlinks;
    return true;
}

bool part::addemitter(const char *tag, int type, int arg1, int arg2)
{
    loopi(numtags) if (!strcmp(tags[i].name, tag))
    {
        if (!links[i].emitter)
            links[i].emitter = new particleemitter;
        particleemitter &p = *links[i].emitter;
        p.type = type;
        p.args[0] = arg1;
        p.args[1] = arg2;
        return true;
    }
    return false;
}

void part::scaleverts(const float scale, const vec3_t &translate)
{
    loopv(meshes)
    {
        mesh &m = *meshes[i];
        loopj(numframes * m.numverts)
        {
            vec3_t &v = m.verts[j];
            if (!index)
                v.add(translate);
            v.mul(scale);
        }
    }
    loopi(numframes * numtags)
    {
        vec3_t &v = tags[i].pos;
        if (!index)
            v.add(translate);
        v.mul(scale);
    }
}

void part::genstrips()
{
    loopv(meshes) meshes[i]->genstrips();
}

void part::getdefaultanim(animstate &as, int anim, int varseed)
{
    as.frame = 0;
    as.range = 1;
}

void part::gentagmatrix(anpos &cur, anpos *prev, float ai_t, int i, GLfloat *matrix)
{
    tag *tag1 = &tags[cur.fr1 * numtags + i];
    tag *tag2 = &tags[cur.fr2 * numtags + i];
#define ip(p1, p2, t) (p1 + t * (p2 - p1))
#define ip_ai_tag(c) ip(ip(tag1p->c, tag2p->c, prev->t), ip(tag1->c, tag2->c, cur.t), ai_t)
    if (prev)
    {
        tag *tag1p = &tags[prev->fr1 * numtags + i];
        tag *tag2p = &tags[prev->fr2 * numtags + i];
        loopj(3) matrix[j] = ip_ai_tag(transform[0][j]); // transform
        loopj(3) matrix[4 + j] = ip_ai_tag(transform[1][j]);
        loopj(3) matrix[8 + j] = ip_ai_tag(transform[2][j]);
        loopj(3) matrix[12 + j] = ip_ai_tag(pos[j]); // position
    }
    else
    {
        loopj(3) matrix[j] = ip(tag1->transform[0][j], tag2->transform[0][j], cur.t); // transform
        loopj(3) matrix[4 + j] = ip(tag1->transform[1][j], tag2->transform[1][j], cur.t);
        loopj(3) matrix[8 + j] = ip(tag1->transform[2][j], tag2->transform[2][j], cur.t);
        loopj(3) matrix[12 + j] = ip(tag1->pos[j], tag2->pos[j], cur.t); // position
    }
#undef ip_ai_tag
#undef ip
    matrix[3] = matrix[7] = matrix[11] = 0.0f;
    matrix[15] = 1.0f;
}

bool part::calcanimstate(int anim, int varseed, float speed, int basetime, dynent *d, animstate &as)
{
    as.anim = anim;
    as.speed = speed <= 0 ? 100.0f : speed;
    as.basetime = basetime;
    if ((anim & ANIM_INDEX) == ANIM_ALL)
    {
        as.frame = 0;
        as.range = numframes;
    }
    else if (anims)
    {
        vector<animinfo> &ais = anims[anim & ANIM_INDEX];
        if (ais.length())
        {
            animinfo &ai = ais[uint(varseed) % ais.length()];
            as.frame = ai.frame;
            as.range = ai.range;
            if (ai.speed > 0)
                as.speed = 1000.0f / ai.speed;
        }
        else
            getdefaultanim(as, anim & ANIM_INDEX, varseed);
    }
    else
        getdefaultanim(as, anim & ANIM_INDEX, varseed);
    if (anim & (ANIM_START | ANIM_END))
    {
        if (anim & ANIM_END)
            as.frame += as.range - 1;
        as.range = 1;
    }

    if (as.frame + as.range > numframes)
    {
        if (as.frame >= numframes)
            return false;
        as.range = numframes - as.frame;
    }

    if (d && index < 2)
    {
        if (d->lastmodel[index] != this || d->lastanimswitchtime[index] == -1 || lastmillis - d->lastrendered > animationinterpolationtime)
        {
            d->current[index] = as;
            d->lastanimswitchtime[index] = lastmillis - animationinterpolationtime * 2;
        }
        else if (d->current[index] != as)
        {
            if (lastmillis - d->lastanimswitchtime[index] > animationinterpolationtime / 2)
                d->prev[index] = d->current[index];
            d->current[index] = as;
            d->lastanimswitchtime[index] = lastmillis;
        }
        d->lastmodel[index] = this;
    }
    return true;
}

void part::render(int anim, int varseed, float speed, int basetime, dynent *d)
{
    if (meshes.empty())
        return;
    animstate as;
    if (!calcanimstate(anim, varseed, speed, basetime, d, as))
        return;

    if (!meshes[0]->dynidx)
        genstrips();

    anpos prev, cur;
    cur.setframes(d && index < 2 ? d->current[index] : as);

    float ai_t = 0;
    bool doai = !(anim & ANIM_NOINTERP) && d && index < 2 && lastmillis - d->lastanimswitchtime[index] < animationinterpolationtime && d->prev[index].range > 0;
    if (doai)
    {
        prev.setframes(d->prev[index]);
        ai_t = (lastmillis - d->lastanimswitchtime[index]) / (float)animationinterpolationtime;
    }

    glPushMatrix();
    glMultMatrixf(vertmodel::matrixstack[vertmodel::matrixpos].v);
    loopv(meshes) meshes[i]->render(as, cur, doai ? &prev : NULL, ai_t);
    glPopMatrix();

    loopi(numtags)
    {
        linkedpart &link = links[i];
        if (!(link.p || link.pos || (anim & ANIM_PARTICLE && link.emitter)))
            continue;

        // render the linked models - interpolate rotation and position of the 'link-tags'
        mat4_t linkmat;
        gentagmatrix(cur, doai ? &prev : NULL, ai_t, i, linkmat.v);

        vertmodel::matrixpos++;
        vertmodel::matrixstack[vertmodel::matrixpos].mul(vertmodel::matrixstack[vertmodel::matrixpos - 1], linkmat);

        if (link.pos)
            *link.pos = vertmodel::matrixstack[vertmodel::matrixpos].gettranslation();

        if (link.p)
        {
            vec3_t oldshadowdir, oldshadowpos;

            if (stenciling)
            {
                oldshadowdir = shadowdir;
                oldshadowpos = shadowpos;
                linkmat.invertnormal(shadowdir);
                linkmat.invertvertex(shadowpos);
            }

            link.p->render(anim, varseed, speed, basetime, d);

            if (stenciling)
            {
                shadowdir = oldshadowdir;
                shadowpos = oldshadowpos;
            }
        }

        if (anim & ANIM_PARTICLE && link.emitter)
        {
            particleemitter &p = *link.emitter;

            if (p.lastemit != basetime)
            {
                p.seed = rnd(0x1000000);
                p.lastemit = basetime;
            }

            particle_emit(p.type, p.args, basetime, p.seed, vertmodel::matrixstack[vertmodel::matrixpos].gettranslation());
        }

        vertmodel::matrixpos--;
    }
}

void part::setanim(int num, int frame, int range, float speed)
{
    if (frame < 0 || frame >= numframes || range <= 0 || frame + range > numframes)
    {
        conoutf("invalid frame %d, range %d in model %s", frame, range, model->loadname);
        return;
    }
    if (!anims)
        anims = new vector<animinfo>[NUMANIMS];
    animinfo &ai = anims[num].add();
    ai.frame = frame;
    ai.range = range;
    ai.speed = speed;
}

void part::begingenshadow()
{
}

void part::endgenshadow()
{
}

void part::blurshadow(const uchar *in, uchar *out, uint size)
{
    static const uint filter3x3[9] =
        {
            1, 2, 1,
            2, 4, 2,
            1, 2, 1};
    static const uint filter3x3sum = 16;
    const uchar *src = in, *prev = in - size, *next = in + size;
    uchar *dst = out;

#define FILTER(c0, c1, c2, c3, c4, c5, c6, c7, c8)                                    \
    {                                                                                 \
        uint c = *src,                                                                \
             val = (filter3x3[0] * (c0) + filter3x3[1] * (c1) + filter3x3[2] * (c2) + \
                    filter3x3[3] * (c3) + filter3x3[4] * (c4) + filter3x3[5] * (c5) + \
                    filter3x3[6] * (c6) + filter3x3[7] * (c7) + filter3x3[8] * (c8)); \
        *dst++ = val / filter3x3sum;                                                  \
        src++;                                                                        \
        prev++;                                                                       \
        next++;                                                                       \
    }

    FILTER(c, c, c, c, c, src[1], c, next[0], next[1]);
    for (uint x = 1; x < size - 1; x++)
        FILTER(c, c, c, src[-1], c, src[1], next[-1], next[0], next[1]);
    FILTER(c, c, c, src[-1], c, c, next[-1], next[0], c);

    for (uint y = 1; y < size - 1; y++)
    {
        FILTER(c, prev[0], prev[1], c, c, src[1], c, next[0], next[1]);
        for (uint x = 1; x < size - 1; x++)
            FILTER(prev[-1], prev[0], prev[1], src[-1], c, src[1], next[-1], next[0], next[1]);
        FILTER(prev[-1], prev[0], c, src[-1], c, c, next[-1], next[0], c);
    }

    FILTER(c, prev[0], prev[1], c, c, src[1], c, c, c);
    for (uint x = 1; x < size - 1; x++)
        FILTER(prev[-1], prev[0], prev[1], src[-1], c, src[1], c, c, c);
    FILTER(prev[-1], prev[0], c, src[-1], c, c, c, c, c);

#undef FILTER
}

void part::genshadow(int aasize, int frame, stream *f)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model->startrender();
    render(ANIM_ALL | ANIM_NOINTERP | ANIM_NOSKIN, 0, 1, lastmillis - frame, NULL);
    model->endrender();

    uchar *pixels = new uchar[2 * aasize * aasize];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, aasize, aasize, GL_RED, GL_UNSIGNED_BYTE, pixels);
#if 0
    SDL_Surface *img = SDL_CreateRGBSurface(SDL_SWSURFACE, aasize, aasize, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
    loopi(aasize*aasize) memset((uchar *)img->pixels + 3*i, pixels[i], 3);
    defformatstring(imgname)("%s_%d.bmp", model->loadname, frame);
    for(char *s; (s = strchr(imgname, '/'));) *s = '_';
    SDL_SaveBMP(img, imgname);
    SDL_FreeSurface(img);
#endif
    if (aasize > 1 << dynshadowsize)
        scaletexture(pixels, aasize, aasize, 1, pixels, 1 << dynshadowsize, 1 << dynshadowsize);

    int texsize = Maths::min(aasize, 1 << dynshadowsize);
    blurshadow(pixels, &pixels[texsize * texsize], texsize);
    if (f)
        f->write(&pixels[texsize * texsize], texsize * texsize);
    createtexture(shadows[frame], texsize, texsize, &pixels[texsize * texsize], 3, true, false, GL_ALPHA);

    delete[] pixels;
}

void part::genshadows(float height, float rad)
{
    if (shadows)
        return;

    char *filename = shadowfile();
    if (filename && loadshadows(filename))
        return;

    shadowrad = rad;
    shadows = new GLuint[numframes];
    glGenTextures(numframes, shadows);

    extern SDL_Surface *screen;
    int aasize = 1 << (dynshadowsize + aadynshadow);
    while (aasize > screen->w || aasize > screen->h)
        aasize /= 2;

    stream *f = filename ? opengzfile(filename, "wb") : NULL;
    if (f)
    {
        shadowheader hdr;
        hdr.size = Maths::min(aasize, 1 << dynshadowsize);
        hdr.frames = numframes;
        hdr.height = height;
        hdr.rad = rad;
        f->putlil(hdr.size);
        f->putlil(hdr.frames);
        f->putlil(hdr.height);
        f->putlil(hdr.rad);
    }

    glViewport(0, 0, aasize, aasize);
    glClearColor(0, 0, 0, 1);
    glDisable(GL_FOG);
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-rad, rad, -rad, rad, 0.15f, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1, -1, 1);

    glTranslatef(0, 0, -height);
    begingenshadow();
    loopi(numframes) genshadow(aasize, i, f);
    endgenshadow();

    glEnable(GL_FOG);
    glViewport(0, 0, screen->w, screen->h);

    if (f)
        delete f;
}

bool part::loadshadows(const char *filename)
{
    stream *f = opengzfile(filename, "rb");
    if (!f)
        return false;
    shadowheader hdr;
    if (f->read(&hdr, sizeof(shadowheader)) != sizeof(shadowheader))
    {
        delete f;
        return false;
    }
    lilswap(&hdr.size, 1);
    lilswap(&hdr.frames, 1);
    if (hdr.size != (1 << dynshadowsize) || hdr.frames != numframes)
    {
        delete f;
        return false;
    }
    lilswap(&hdr.height, 1);
    lilswap(&hdr.rad, 1);

    uchar *buf = new uchar[hdr.size * hdr.size * hdr.frames];
    if (f->read(buf, hdr.size * hdr.size * hdr.frames) != hdr.size * hdr.size * hdr.frames)
    {
        delete f;
        delete[] buf;
        return false;
    }

    shadowrad = hdr.rad;
    shadows = new GLuint[hdr.frames];
    glGenTextures(hdr.frames, shadows);

    loopi(hdr.frames) createtexture(shadows[i], hdr.size, hdr.size, &buf[i * hdr.size * hdr.size], 3, true, false, GL_ALPHA);

    delete[] buf;
    delete f;

    return true;
}

void part::rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw)
{
    if (!shadows)
        return;
    animstate as;
    if (!calcanimstate(anim, varseed, speed, basetime, NULL, as))
        return;
    anpos cur;
    cur.setframes(as);

    GLuint id = shadows[cur.fr1];
    if (id != vertmodel::lasttex)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        vertmodel::lasttex = id;
    }

    if (vertmodel::enabledepthmask)
    {
        glDepthMask(GL_FALSE);
        vertmodel::enabledepthmask = false;
    }
    if (vertmodel::enablealphatest)
    {
        glDisable(GL_ALPHA_TEST);
        vertmodel::enablealphatest = false;
    }
    if (!vertmodel::enablealphablend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        vertmodel::enablealphablend = true;
    }

    yaw *= RAD;
    float c = cosf(yaw), s = sinf(yaw);
    float x1 = -shadowrad, x2 = shadowrad;
    float y1 = -shadowrad, y2 = shadowrad;

    if (dynshadowquad)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(x1 * c - y1 * s + o.x, y1 * c + x1 * s + o.y, o.z);
        glTexCoord2f(1, 1);
        glVertex3f(x2 * c - y1 * s + o.x, y1 * c + x2 * s + o.y, o.z);
        glTexCoord2f(1, 0);
        glVertex3f(x2 * c - y2 * s + o.x, y2 * c + x2 * s + o.y, o.z);
        glTexCoord2f(0, 0);
        glVertex3f(x1 * c - y2 * s + o.x, y2 * c + x1 * s + o.y, o.z);
        glEnd();
        xtraverts += 4;
        return;
    }

    if (!vertmodel::enableoffset)
    {
        enablepolygonoffset(GL_POLYGON_OFFSET_FILL);
        vertmodel::enableoffset = true;
    }
    if (vertmodel::lastvertexarray)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        vertmodel::lastvertexarray = NULL;
    }
    if (vertmodel::lasttexcoordarray)
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        vertmodel::lasttexcoordarray = NULL;
    }

    vec3_t texgenS, texgenT;
    texgenS.x = c / (2 * shadowrad);
    texgenS.y = s / (2 * shadowrad);
    texgenS.z = -(x1 * c - y2 * s + o.x) * texgenS.x - (y2 * c + x1 * s + o.y) * texgenS.y;

    texgenT.x = s / (2 * shadowrad);
    texgenT.y = -c / (2 * shadowrad);
    texgenT.z = -(x1 * c - y2 * s + o.x) * texgenT.x - (y2 * c + x1 * s + o.y) * texgenT.y;

    ::rendershadow(int(floor(o.x - shadowrad)), int(floor(o.y - shadowrad)), int(ceil(o.x + shadowrad)), int(ceil(o.y + shadowrad)), texgenS, texgenT);
}

char *part::shadowfile()
{
    if (!saveshadows || !filename)
        return NULL;

    static string s;
    char *dir = strrchr(filename, PATHDIV);
    if (!dir)
        s[0] = '\0';
    else
        copystring(s, filename, dir - filename + 2);
    concatstring(s, "shadows.dat");
    return s;
}

float part::calcradius()
{
    float rad = 0;
    loopv(meshes) rad = Maths::max(rad, meshes[i]->calcradius());
    return rad;
}

void part::calcneighbors()
{
    loopv(meshes) meshes[i]->calcneighbors();
}

void part::calcbbs()
{
    loopv(meshes) meshes[i]->calcbbs();
}

//////////////////////////////////////////////////////////////////
vertmodel::vertmodel(const char *name) : loaded(false)
{
    loadname = newstring(name);
}

vertmodel::~vertmodel()
{
    delete[] loadname;
    parts.deletecontents();
}

char *vertmodel::name() { return loadname; }

void vertmodel::cleanup()
{
    loopv(parts) parts[i]->cleanup();
}

bool vertmodel::link(part *link, const char *tag, vec3_t *pos)
{
    loopv(parts) if (parts[i]->link(link, tag, pos)) return true;
    return false;
}

void vertmodel::setskin(int tex)
{
    // if(parts.length()!=1 || parts[0]->meshes.length()!=1) return;
    if (parts.length() < 1 || parts[0]->meshes.length() < 1)
        return;
    mesh &m = *parts[0]->meshes[0];
    m.tex = tex;
}

void vertmodel::genshadows(float height, float rad)
{
    if (parts.length() > 1)
        return;
    parts[0]->genshadows(height, rad);
}

bool vertmodel::hasshadows()
{
    return parts.length() == 1 && parts[0]->shadows;
}

float vertmodel::calcradius()
{
    return parts.empty() ? 0.0f : parts[0]->calcradius();
}

void vertmodel::calcneighbors()
{
    loopv(parts) parts[i]->calcneighbors();
}

void vertmodel::calcbbs()
{
    loopv(parts) parts[i]->calcbbs();
}

void vertmodel::startrender()
{
    enablealphablend = enablealphatest = enableoffset = false;
    enabledepthmask = true;
    lasttex = 0;
    lastalphatest = -1;
    lastvertexarray = lasttexcoordarray = lastcolorarray = NULL;
}

void vertmodel::endrender()
{
    if (enablealphablend)
        glDisable(GL_BLEND);
    if (enablealphatest)
        glDisable(GL_ALPHA_TEST);
    if (!enabledepthmask)
        glDepthMask(GL_TRUE);
    if (lastvertexarray)
        glDisableClientState(GL_VERTEX_ARRAY);
    if (lasttexcoordarray)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (lastcolorarray)
        glDisableClientState(GL_COLOR_ARRAY);
    if (enableoffset)
        disablepolygonoffset(GL_POLYGON_OFFSET_FILL);
}

bool vertmodel::enablealphablend = false, vertmodel::enablealphatest = false, vertmodel::enabledepthmask = true, vertmodel::enableoffset = false;
GLuint vertmodel::lasttex = 0;
float vertmodel::lastalphatest = -1;
void *vertmodel::lastvertexarray = NULL, *vertmodel::lasttexcoordarray = NULL, *vertmodel::lastcolorarray = NULL;
mat4_t vertmodel::matrixstack[32];
int vertmodel::matrixpos = 0;

VARF(mdldyncache, 1, 2, 32, vertmodel::dynalloc.resize(mdldyncache << 20));
VARF(mdlstatcache, 1, 1, 32, vertmodel::statalloc.resize(mdlstatcache << 20));

modelcache vertmodel::dynalloc(mdldyncache << 20), vertmodel::statalloc(mdlstatcache << 20);
