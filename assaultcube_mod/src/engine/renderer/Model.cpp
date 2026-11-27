#include "pch.h"
#include "Model.h"
#include "vert_animation.h"
#include "render_particles.h"
#include "shadow.h"
#include "render_cubes.h"
#include "lib/glad/glad.h"

// i wish there was a better api style to handle this for more client
//  side things
#include "client/client.h"

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

ModelPart::ModelPart() 
    : filename(NULL), numtags(0), shadowrad(0) 
{

}
ModelPart::~ModelPart()
{
    DELETEA(filename);
    // m_meshes.deletecontents();
    for (size_t i = 0; i < m_meshes.size(); i++) {
        if (m_meshes[i]) {
            delete m_meshes[i];
        }
    }
    // DELETEA(anims);
    if (!m_shadows.empty()) {
        glDeleteTextures(numframes, &m_shadows[0]);
    }
    // DELETEA(shadows);
}

void ModelPart::cleanup()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->cleanup();
    }
    if (!m_shadows.empty()) {
        glDeleteTextures(numframes, &m_shadows[0]);
        std::vector<GLuint> shads;
        m_shadows.swap(shads);
    }
}

bool ModelPart::link(ModelPart* pLink, const char *tag, vec3_t *pos)
{
    for (int i = 0; i < numtags; i++) {
        if (!strcmp(m_tags[i].name, tag)) {
            m_links[i].p = pLink;
            m_links[i].pos = pos;
            return true;
        }
    }
    return false;
}

bool ModelPart::gentag(const char *name, int *verts, int numverts, Mesh* m)
{
    if (!m) {
        if (m_meshes.empty()) {
            return false;
        }
        m = m_meshes[0];
    }
    if (numverts < 1) {
        return false;
    }
    for (int i = 0; i < numverts; i++) {
        if (verts[i] < 0 || verts[i] > m->numverts) {
            return false;
        }
    }

    m_tags.resize((numtags + 1) * numframes);
    m_tags[numtags].name = newstring(name);
    for (int i = 0; i < numframes; i++) {
        tag* t = &m_tags[(numtags + 1) * i + numtags];
        t->pos = m->m_verts[m->numverts * i + verts[0]];
        if (numverts > 1) {
            for (int j = 1; j < numverts; j++) {
                t->pos.add(m->m_verts[m->numverts * i + verts[j]]);
            }
            t->pos.div(numverts);
        }
        t->identity();
    }
    for (int i = 0; i < numtags; i++) {
        m_tags[i].name = NULL;
    }
    numtags++;

    m_links.resize(numtags);
    return true;
}

bool ModelPart::addemitter(const char *tag, int type, int arg1, int arg2)
{
    for (int i = 0; i < numtags; i++) {
        if (!strcmp(m_tags[i].name, tag)) {
            if (!m_links[i].emitter) {
                m_links[i].emitter = new particleemitter;
            }
            particleemitter &p = *m_links[i].emitter;
            p.type = type;
            p.args[0] = arg1;
            p.args[1] = arg2;
            return true;
        }
    }
    return false;
}

void ModelPart::scaleverts(const float scale, const vec3_t &translate)
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        Mesh& m = *m_meshes[i];
        loopj(numframes * m.numverts)
        {
            vec3_t &v = m.m_verts[j];
            if (!index)
                v.add(translate);
            v.mul(scale);
        }
    }
    for (int i = 0; i < (numframes * numtags); i++) {
        vec3_t &v = m_tags[i].pos;
        if (!index)
            v.add(translate);
        v.mul(scale);
    }
}

void ModelPart::genstrips()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->genstrips();
    }
}

void ModelPart::getdefaultanim(animstate &as, int anim, int varseed)
{
    as.frame = 0;
    as.range = 1;
}

void ModelPart::gentagmatrix(anpos &cur, anpos *prev, float ai_t, int i, GLfloat *matrix)
{
    tag *tag1 = &m_tags[cur.fr1 * numtags + i];
    tag *tag2 = &m_tags[cur.fr2 * numtags + i];
#define ip(p1, p2, t) (p1 + t * (p2 - p1))
#define ip_ai_tag(c) ip(ip(tag1p->c, tag2p->c, prev->t), ip(tag1->c, tag2->c, cur.t), ai_t)
    if (prev)
    {
        tag *tag1p = &m_tags[prev->fr1 * numtags + i];
        tag *tag2p = &m_tags[prev->fr2 * numtags + i];
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

bool ModelPart::calcanimstate(int anim, int varseed, float speed, int basetime, dynent *d, animstate &as)
{
    as.anim = anim;
    as.speed = speed <= 0 ? 100.0f : speed;
    as.basetime = basetime;
    if ((anim & ANIM_INDEX) == ANIM_ALL)
    {
        as.frame = 0;
        as.range = numframes;
    }
    else if (!m_anims.empty()) {
        std::vector<animinfo>& ais = m_anims[anim & ANIM_INDEX];
        if (ais.size()) {
            animinfo& ai = ais[uint(varseed) % ais.size()];
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

void ModelPart::render(int anim, int varseed, float speed, int basetime, dynent *d)
{
    if (m_meshes.empty())
        return;
    animstate as;
    if (!calcanimstate(anim, varseed, speed, basetime, d, as))
        return;

    if (m_meshes[0]->m_dynidx.empty())
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
    glMultMatrixf(Model::matrixstack[Model::matrixpos].v);
    for (size_t i = 0; i < m_meshes.size(); i++) { 
        m_meshes[i]->render(as, cur, doai ? &prev : NULL, ai_t);
    }
    glPopMatrix();

    for (int i = 0; i < numtags; i++) {
        linkedpart& link = m_links[i];
        if (!(link.p || link.pos || (anim & ANIM_PARTICLE && link.emitter)))
            continue;

        // render the linked models - interpolate rotation and position of the 'link-tags'
        mat4_t linkmat;
        gentagmatrix(cur, doai ? &prev : NULL, ai_t, i, linkmat.v);

        Model::matrixpos++;
        Model::matrixstack[Model::matrixpos].mul(Model::matrixstack[Model::matrixpos - 1], linkmat);

        if (link.pos)
            *link.pos = Model::matrixstack[Model::matrixpos].gettranslation();

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

            particle_emit(p.type, p.args, basetime, p.seed, Model::matrixstack[Model::matrixpos].gettranslation());
        }

        Model::matrixpos--;
    }
}

void ModelPart::setanim(int num, int frame, int range, float speed)
{
    if (frame < 0 || frame >= numframes || range <= 0 || frame + range > numframes)
    {
        conoutf("invalid frame %d, range %d in model %s", frame, range, model->loadname);
        return;
    }
    if (m_anims.empty()) {
        // anims = new vector<animinfo>[NUMANIMS];
        m_anims.resize(NUMANIMS);
    }
    animinfo ai;// = m_anims[num].add();
    ai.frame = frame;
    ai.range = range;
    ai.speed = speed;
    m_anims[num].push_back(ai);
}

void ModelPart::begingenshadow()
{
}

void ModelPart::endgenshadow()
{
}

void ModelPart::blurshadow(const uchar *in, uchar *out, uint size)
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

void ModelPart::genshadow(int aasize, int frame, stream *f)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model->startrender();
    render(ANIM_ALL | ANIM_NOINTERP | ANIM_NOSKIN, 0, 1, lastmillis - frame, NULL);
    model->endrender();

    uchar *pixels = new uchar[2 * aasize * aasize];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, aasize, aasize, GL_RED, GL_UNSIGNED_BYTE, pixels);

    if (aasize > 1 << dynshadowsize)
        scaletexture(pixels, aasize, aasize, 1, pixels, 1 << dynshadowsize, 1 << dynshadowsize);

    int texsize = Maths::min(aasize, 1 << dynshadowsize);
    blurshadow(pixels, &pixels[texsize * texsize], texsize);
    if (f)
        f->write(&pixels[texsize * texsize], texsize * texsize);
    createtexture(m_shadows[frame], texsize, texsize, &pixels[texsize * texsize], 3, true, false, GL_ALPHA);

    delete[] pixels;
}

void ModelPart::genshadows(float height, float rad)
{
    if (!m_shadows.empty()) {
        return;
    }

    char *filename = shadowfile();
    if (filename && loadshadows(filename))
        return;

    shadowrad = rad;
            
    m_shadows.resize(numframes);
    glGenTextures(numframes, &m_shadows[0]);

    SDL_Surface* pScreen = g_client.window.getSDLSurface();
    int aasize = 1 << (dynshadowsize + aadynshadow);
    while (aasize > pScreen->w || aasize > pScreen->h) {
        aasize /= 2;
    }

    stream *f = filename ? opengzfile(filename, "wb") : NULL;
    if (f) {
        ShadowHeader hdr;
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

bool ModelPart::loadshadows(const char *filename)
{
    stream *f = opengzfile(filename, "rb");
    if (!f)
        return false;
    ShadowHeader hdr;
    if (f->read(&hdr, sizeof(ShadowHeader)) != sizeof(ShadowHeader)) {
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
    m_shadows.resize(hdr.frames);
    glGenTextures(hdr.frames, &m_shadows[0]);

    loopi(hdr.frames) createtexture(m_shadows[i], hdr.size, hdr.size, &buf[i * hdr.size * hdr.size], 3, true, false, GL_ALPHA);

    delete[] buf;
    delete f;

    return true;
}

void ModelPart::rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw)
{
    if (m_shadows.empty()) {
        return;
    }
    animstate as;
    if (!calcanimstate(anim, varseed, speed, basetime, NULL, as))
        return;
    anpos cur;
    cur.setframes(as);

    GLuint id = m_shadows[cur.fr1];
    if (id != Model::lasttex)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        Model::lasttex = id;
    }

    if (Model::enabledepthmask)
    {
        glDepthMask(GL_FALSE);
        Model::enabledepthmask = false;
    }
    if (Model::enablealphatest)
    {
        glDisable(GL_ALPHA_TEST);
        Model::enablealphatest = false;
    }
    if (!Model::enablealphablend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Model::enablealphablend = true;
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

    if (!Model::enableoffset)
    {
        enablepolygonoffset(GL_POLYGON_OFFSET_FILL);
        Model::enableoffset = true;
    }
    if (Model::lastvertexarray)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        Model::lastvertexarray = NULL;
    }
    if (Model::lasttexcoordarray)
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        Model::lasttexcoordarray = NULL;
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

char *ModelPart::shadowfile()
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

float ModelPart::calcradius()
{
    float rad = 0;
    for (size_t i = 0; i < m_meshes.size(); i++) {
        rad = Maths::max(rad, m_meshes[i]->calcradius());
    }
    return rad;
}

void ModelPart::calcneighbors()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->calcneighbors();
    }
}

void ModelPart::calcbbs()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->calcbbs();
    }
}

//////////////////////////////////////////////////////////////////
Model::Model()
{

}

Model::Model(const char *name, ModelTypes_t mtype) 
    : cullface(true), vertexlight(false),  alphablend(false), alphatest(0.9f), translucency(0.25f), scale(1), 
        radius(0), shadowdist(0), translate(0, 0, 0), cachelimit(8), batch(-1), loaded(false)
{
    m_type = mtype;
    loadname = newstring(name);
}

Model::~Model()
{
    delete[] loadname;
    parts.deletecontents();
}

char *Model::name() 
{ 
    return loadname; 
}

void Model::render(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, float pitch, dynent *d, modelattach *a, float scale)
{
    if (m_type == ModelTypes_t::NONE) {
        return;
    }

    if(!loaded) {
        return;
    }

    if (m_type == ModelTypes_t::MDL_MD2) {
        if(a) {
            for(int i = 0; a[i].tag; i++) {
                if(a[i].pos) link(NULL, a[i].tag, a[i].pos);
            }
        }

        if(!cullface) glDisable(GL_CULL_FACE);
        else if(anim&ANIM_MIRROR) glCullFace(GL_BACK);

        if(stenciling && !parts[0]->index) {
            shadowdir = vec3_t(0, 1/SQRT2, -1/SQRT2);
            shadowdir.rotate_around_z((-shadowyaw-yaw-180.0f)*RAD);
            shadowdir.rotate_around_y(-pitch*RAD);
            (shadowpos = shadowdir).mul(shadowdist);
        }

        modelpos = o;
        modelyaw = yaw;
        modelpitch = pitch;

        matrixpos = 0;
        matrixstack[0].identity();
        matrixstack[0].translate(o);
        matrixstack[0].rotate_around_z((yaw+180)*RAD);
        matrixstack[0].rotate_around_y(-pitch*RAD);
        if(anim&ANIM_MIRROR || scale!=1) matrixstack[0].scale(scale, anim&ANIM_MIRROR ? -scale : scale, scale);
        parts[0]->render(anim, varseed, speed, basetime, d);

        if(!cullface) glEnable(GL_CULL_FACE);
        else if(anim&ANIM_MIRROR) glCullFace(GL_FRONT);

        if(a) for(int i = 0; a[i].tag; i++)
        {
            if(a[i].pos) link(NULL, a[i].tag, NULL);

            Model* m = (Model*)a[i].pModel;
            if(!m) continue;
            m->parts[0]->index = parts.length()+i;
            m->setskin();
            m->render(anim, varseed, speed, basetime, o, yaw, pitch, d, NULL, scale);
        }

        if(d) d->lastrendered = lastmillis;
    }
    else if (m_type == ModelTypes_t::MDL_MD3) {
        if(a) {
            for(int i = 0; a[i].tag; i++) {
                Model* m = (Model*)a[i].pModel;
                if(!m)
                {
                    if(a[i].pos) link(NULL, a[i].tag);
                    continue;
                }
                ModelPart* p = m->parts[0];
                if(link(p, a[i].tag, a[i].pos)) p->index = parts.length()+i;
            }
        }

        if(!cullface) glDisable(GL_CULL_FACE);
        else if(anim&ANIM_MIRROR) glCullFace(GL_BACK);

        if(stenciling) {
            shadowdir = vec3_t(0, 1/SQRT2, -1/SQRT2);
            shadowdir.rotate_around_z((-shadowyaw-yaw-180.0f)*RAD);
            shadowdir.rotate_around_y(-pitch*RAD);
            (shadowpos = shadowdir).mul(shadowdist);
        }

        modelpos = o;
        modelyaw = yaw;
        modelpitch = pitch;

        matrixpos = 0;
        matrixstack[0].identity();
        matrixstack[0].translate(o);
        matrixstack[0].rotate_around_z((yaw+180)*RAD);
        matrixstack[0].rotate_around_y(-pitch*RAD);
        if(anim&ANIM_MIRROR || scale!=1) matrixstack[0].scale(scale, anim&ANIM_MIRROR ? -scale : scale, scale);
        parts[0]->render(anim, varseed, speed, basetime, d);

        if(!cullface) glEnable(GL_CULL_FACE);
        else if(anim&ANIM_MIRROR) glCullFace(GL_FRONT);

        if(a) { 
            for(int i = 0; a[i].tag; i++) { 
                link(NULL, a[i].tag); 
            } 
        }

        if(d) { 
            d->lastrendered = lastmillis; 
        }
    }
}

void Model::cleanup()
{
    loopv(parts) parts[i]->cleanup();
}

bool Model::link(ModelPart* pLink, const char *tag, vec3_t *pos)
{
    loopv(parts) if (parts[i]->link(pLink, tag, pos)) return true;
    return false;
}

void Model::setskin(int tex)
{
    // if(parts.length()!=1 || parts[0]->m_meshes.size()!=1) return;
    if (parts.length() < 1 || parts[0]->m_meshes.size() < 1)
        return;
    Mesh& m = *parts[0]->m_meshes[0];
    m.tex = tex;
}

void Model::genshadows(float height, float rad)
{
    if (parts.length() > 1)
        return;
    parts[0]->genshadows(height, rad);
}

bool Model::hasshadows()
{
    return parts.length() == 1 && parts[0]->m_shadows.empty() == false;
}

float Model::calcradius()
{
    return parts.empty() ? 0.0f : parts[0]->calcradius();
}

void Model::calcneighbors()
{
    loopv(parts) parts[i]->calcneighbors();
}

void Model::calcbbs()
{
    loopv(parts) parts[i]->calcbbs();
}

void Model::startrender()
{
    enablealphablend = enablealphatest = enableoffset = false;
    enabledepthmask = true;
    lasttex = 0;
    lastalphatest = -1;
    lastvertexarray = lasttexcoordarray = lastcolorarray = NULL;
}

void Model::endrender()
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

bool Model::enablealphablend = false, Model::enablealphatest = false, Model::enabledepthmask = true, Model::enableoffset = false;
GLuint Model::lasttex = 0;
float Model::lastalphatest = -1;
void *Model::lastvertexarray = NULL, *Model::lasttexcoordarray = NULL, *Model::lastcolorarray = NULL;
mat4_t Model::matrixstack[32];
int Model::matrixpos = 0;

VARF(mdldyncache, 1, 2, 32, Model::dynalloc.resize(mdldyncache << 20));
VARF(mdlstatcache, 1, 1, 32, Model::statalloc.resize(mdlstatcache << 20));

ModelCache Model::dynalloc(mdldyncache << 20), Model::statalloc(mdlstatcache << 20);
