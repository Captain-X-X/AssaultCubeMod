#include "pch.h"
#include "ModelMD2.h"
#include "texture.h"
#include "render_model.h"

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

ModelMD2* loadingmd2 = 0;

struct md2_header
{
    int magic;
    int version;
    int skinwidth, skinheight;
    int framesize;
    int numskins, numvertices, numtexcoords;
    int numtriangles, numglcommands, numframes;
    int offsetskins, offsettexcoords, offsettriangles;
    int offsetframes, offsetglcommands, offsetend;
};

struct md2_vertex
{
    uchar vertex[3], normalindex;
};

struct md2_frame
{
    float      scale[3];
    float      translate[3];
    char       name[16];
};

class MD2Part : public ModelPart
{
public:
    void gentcverts(std::vector<int>& glcommands, std::vector<Vector2>& tcverts, std::vector<ushort> &vindexes, std::vector<tri>& tris)
    {
        hashtable<ivec, int> tchash;
        std::vector<ushort> idxs;
        for(int* command = glcommands.data(); (*command)!=0;) {
            int numvertex = *command++;
            bool isfan = numvertex<0;
            if(isfan) numvertex = -numvertex;
            idxs.clear();
            for (int i = 0; i < numvertex; i++) {
                union { int i; float f; } u, v;
                u.i = *command++;
                v.i = *command++;
                int vindex = *command++;
                ivec tckey(u.i, v.i, vindex);
                int *idx = tchash.access(tckey);
                if(!idx) {
                    idx = &tchash[tckey];
                    *idx = tcverts.size();
                    Vector2 uv;// = tcverts.add();
                    uv.x = u.f;
                    uv.y = v.f;
                    vindexes.push_back((ushort)vindex);
                    tcverts.push_back(uv);
                }
                idxs.push_back(*idx);
            }
            for (int i = 0; i < numvertex - 2; i++) {
                tri tri;// = tris.add();
                if(isfan) {
                    tri.vert[0] = idxs[0];
                    tri.vert[1] = idxs[i+1];
                    tri.vert[2] = idxs[i+2];
                }
                else loopk(3) tri.vert[k] = idxs[i&1 && k ? i+(1-(k-1))+1 : i+k];
                tris.push_back(tri);
            }
        }
    }

    bool load(char *path)
    {
        if(filename) {
            return true;
        }

        std::ifstream fileIn(path, std::ios::in);
        if (!fileIn.is_open()) {
            return false;
        }

        md2_header header;
        fileIn.read((char*)&header, sizeof(md2_header));
        lilswap((int *)&header, sizeof(md2_header)/sizeof(int));

        if(header.magic!=844121161 || header.version!=8) {
            fileIn.close();
            return false;
        }

        numframes = header.numframes;

        m_meshes.push_back(new Mesh);
        Mesh& m = *m_meshes.back();
        m.owner = this;

        std::vector<int> glCommands;
        glCommands.resize(header.numglcommands);
        fileIn.seekg(header.offsetglcommands);
        fileIn.read((char*)glCommands.data(), sizeof(int) * header.numglcommands);
        lilswap(glCommands.data(), glCommands.size());

        if(glCommands.size() < static_cast<size_t>(header.numglcommands)) {
            memset(&glCommands[glCommands.size()], 0, (header.numglcommands-glCommands.size())*sizeof(int));
        }

        std::vector<ushort> vgen;
        gentcverts(glCommands, m.m_uvs, vgen, m.m_tris);

        m.numverts = m.m_uvs.size();
        m.numtris = m.m_tris.size();
        m.m_verts.resize(m.numverts * numframes + 1);

        std::vector<md2_vertex> tmpverts;
        tmpverts.resize(header.numvertices);

        int frame_offset = header.offsetframes;
        vec3_t* curvert = m.m_verts.data();
        for (int i = 0; i < header.numframes; i++) {
            md2_frame frame;
            fileIn.seekg(frame_offset);
            fileIn.read((char*)&frame, sizeof(md2_frame));
            lilswap((float*)&frame, 6);

            fileIn.read((char*)&tmpverts[0], sizeof(md2_vertex) * header.numvertices);
            for (int j = 0; j < m.numverts; j++) {
                const md2_vertex &v = tmpverts[vgen[j]];
                *curvert++ = vec3_t(
                     ( v.vertex[0] * frame.scale[0] + frame.translate[0] ), // X
                    -( v.vertex[1] * frame.scale[1] + frame.translate[1] ), // Y
                     ( v.vertex[2] * frame.scale[2] + frame.translate[2] )  // Z
                );
            }
            frame_offset += header.framesize;
            tmpverts.clear();
        }
        // delete file;
        fileIn.close();

        filename = newstring(path);
        return true;
    }

    void getdefaultanim(animstate &as, int anim, int varseed)
    {
        //                      0   1   2   3   4   5   6   7   8   9  10  11  12   13  14  15  16  17 18  19  20   21  21  23  24
        //                      I   R   A   P   P   P   J   L   F   S   T   W   P   CI  CW  CA  CP  CD  D   D   D   LD  LD  LD   F
        static int frame[] =  { 0,  40, 46, 54, 58, 62, 66, 69, 72, 84, 95, 112,123,135,154,160,169,173,178,184,190,183,189,197, 0 };
        static int range[] =  { 40, 6,  8,  4,  4,  4,  3,  3,  12, 11, 17, 11, 12, 19, 6,  9,  4,  5,  6,  6,  8,  1,  1,  1,   7 };
        static int animfr[] = { 0,  1,  2,  3,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 24 };

        if((size_t)anim >= sizeof(animfr)/sizeof(animfr[0])) {
            as.frame = 0;
            as.range = 1;
            return;
        }
        int n = animfr[anim];
        if(anim==ANIM_PAIN || anim==ANIM_DEATH || anim==ANIM_LYING_DEAD) n += uint(varseed)%3;
        as.frame = frame[n];
        as.range = range[n];
    }

    void begingenshadow()
    {
        Model::matrixpos = 0;
        Model::matrixstack[0].identity();
        Model::matrixstack[0].rotate_around_z(180*RAD);
    }
};

/*
* ==========
* MD2 model loader
* ==========
*/
ModelMD2::ModelMD2(const char *name) 
    : Model(name, ModelTypes_t::MDL_MD2) 
{

}

ModelMD2::~ModelMD2() 
{ 

}

void ModelMD2::rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a)
{
    parts[0]->rendershadow(anim, varseed, speed, basetime, o, yaw);
    if(a) for(int i = 0; a[i].tag; i++)
    {
        Model* m = (Model*)a[i].pModel;
        if(!m) continue;
        ModelPart* p = m->parts[0];
        p->rendershadow(anim, varseed, speed, basetime, o, yaw);
    }
}

bool ModelMD2::load()
{
    if(loaded) return true;
    MD2Part& mdl = *new MD2Part();
    parts.add(&mdl);
    mdl.model = this;
    mdl.index = 0;
    const char *pname = parentdir(loadname);
    defformatstring(name1)("packages/models/%s/tris.md2", loadname);
    if(!mdl.load(path(name1)))
    {
        formatstring(name1)("packages/models/%s/tris.md2", pname);    // try md2 in parent folder (vert sharing)
        if(!mdl.load(path(name1))) return false;
    }
    Texture *skin;
    loadskin(loadname, pname, skin);
    for (size_t i = 0; i < mdl.m_meshes.size(); i++) { 
        mdl.m_meshes[i]->skin  = skin;
    }
    if(skin==notexture) conoutf(_("could not load model skin for %s"), name1);
    loadingmd2 = this;
    defformatstring(name2)("packages/models/%s/md2.cfg", loadname);
    per_idents = false;
    neverpersist = true;
    if(!execfile(name2))
    {
        formatstring(name2)("packages/models/%s/md2.cfg", pname);
        execfile(name2);
    }
    neverpersist = false;
    per_idents = true;
    loadingmd2 = 0;
    loopv(parts) parts[i]->scaleverts(scale/16.0f, vec3_t(translate.x, -translate.y, translate.z));
    radius = calcradius();
    if(shadowdist) calcneighbors();
    calcbbs();
    return loaded = true;
}

void md2anim(char *anim, int *frame, int *range, float *speed)
{
    if(!loadingmd2 || loadingmd2->parts.empty()) { conoutf("not loading an md2"); return; }
    int num = findanim(anim);
    if(num<0) { conoutf("could not find animation %s", anim); return; }
    loadingmd2->parts.last()->setanim(num, *frame, *range, *speed);
}

void md2tag(char *name, char *vert1, char *vert2, char *vert3, char *vert4)
{
    if(!loadingmd2 || loadingmd2->parts.empty()) { conoutf("not loading an md2"); return; }
    ModelPart& mdl = *loadingmd2->parts.last();
    int indexes[4] = { -1, -1, -1, -1 }, numverts = 0;
    loopi(4)
    {
        char *vert = NULL;
        switch(i)
        {
            case 0: vert = vert1; break;
            case 1: vert = vert2; break;
            case 2: vert = vert3; break;
            case 3: vert = vert4; break;
        }
        if(!vert[0]) break;
        if(isdigit(vert[0])) indexes[i] = ATOI(vert);
        else
        {
            int axis = 0, dir = 1;
            for(char *s = vert; *s; s++) switch(*s)
            {
                case '+': dir = 1; break;
                case '-': dir = -1; break;
                case 'x':
                case 'X': axis = 0; break;
                case 'y':
                case 'Y': axis = 1; break;
                case 'z':
                case 'Z': axis = 2; break;
            }
            if(!mdl.m_meshes.empty()) indexes[i] = mdl.m_meshes[0]->findvert(axis, dir);
        }
        if(indexes[i] < 0) { conoutf("could not find vertex %s", vert); return; }
        numverts = i + 1;
    }
    if(!mdl.gentag(name, indexes, numverts)) { conoutf("could not generate tag %s", name); return; }
}

void md2emit(char *tag, int *type, int *arg1, int *arg2)
{
    if(!loadingmd2 || loadingmd2->parts.empty()) { conoutf("not loading an md2"); return; };
    ModelPart& mdl = *loadingmd2->parts.last();
    if(!mdl.addemitter(tag, *type, *arg1, *arg2)) { conoutf("could not find tag %s", tag); return; }
}

COMMAND(md2anim, "siif");
COMMAND(md2tag, "sssss");
COMMAND(md2emit, "siii");
