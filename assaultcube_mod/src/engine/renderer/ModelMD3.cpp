#include "pch.h"
#include "ModelMD3.h"
#include "cube.h"
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

string md3dir;
ModelMD3* loadingmd3 = NULL;

struct MD3Tag_s {
    char name[64];
    Vector3 pos;
    float rotation[3][3];
};

struct md3vertex {
    short vertex[3];
    short normal;
};

struct md3triangle {
    int vertexindices[3];
};

struct MD3Header_s {
    char id[4];
    int version;
    char name[64];
    int flags;
    int numframes, numtags, nummeshes, numskins;
    int ofs_frames, ofs_tags, ofs_meshes, ofs_eof; // offsets
};

struct md3meshheader {
    char id[4];
    char name[64];
    int flags;
    int numframes, numshaders, numvertices, numtriangles;
    int ofs_triangles, ofs_shaders, ofs_uv, ofs_vertices, meshsize; // offsets
};

class MD3Part : public ModelPart
{
public:
    bool loadHeader(std::ifstream& file, MD3Header_s& header)
    {
        file.read((char*)&header, sizeof(MD3Header_s));
        lilswap(&header.version, 1);
        lilswap(&header.flags, 9);
        if(strncmp(header.id, "IDP3", 4) != 0 || header.version != 15) { // header check 
            return false;
        }
        return true;
    }

    bool load(char *path)
    {
        if(filename) {
            return true;
        }

        MD3Header_s header;

        std::ifstream fileIn(path, std::ios::in);
        if (!fileIn.is_open()) {
            return false;
        }

        if (!loadHeader(fileIn, header)) {
            return false;
        }

        numframes = header.numframes;
        numtags = header.numtags;
        if(numtags) {
            m_tags.resize(numframes*numtags);
            fileIn.seekg(header.ofs_tags);
            MD3Tag_s tag;

            for (int i = 0; i < (header.numframes * header.numtags); i++) {
                fileIn.read((char*)&tag, sizeof(MD3Tag_s));
                lilswap((float *)&tag.pos, 12);
                if(tag.name[0] && i<header.numtags) {
                    m_tags[i].name = newstring(tag.name);
                }
                m_tags[i].pos = vec3_t(tag.pos.y, tag.pos.x, tag.pos.z);
                memcpy(m_tags[i].transform, tag.rotation, sizeof(tag.rotation));
                // undo the x/y swap
                for (int j = 0; j < 3; j++) swap(m_tags[i].transform[0][j], m_tags[i].transform[1][j]);
                // then restore it
                for (int j = 0; j < 3; j++) swap(m_tags[i].transform[j][0], m_tags[i].transform[j][1]);
            }
            m_links.resize(numtags);
        }

        int mesh_offset = header.ofs_meshes;
        for (int i = 0; i < header.nummeshes; i++) {
            md3meshheader mheader;
            fileIn.seekg(mesh_offset);
            fileIn.read((char*)&mheader, sizeof(md3meshheader));
            lilswap(&mheader.flags, 10);

            if(mheader.numtriangles <= 0) {
                mesh_offset += mheader.meshsize;
                continue;
            }

            m_meshes.push_back(new Mesh);
            Mesh& m = *m_meshes.back();
            m.owner = this;
            m.name = newstring(mheader.name);

            m.numtris = mheader.numtriangles;
            m.m_tris.resize(m.numtris);
            fileIn.seekg(mesh_offset + mheader.ofs_triangles);
            for (int j = 0; j < mheader.numtriangles; j++) {
                md3triangle tri;
                memset((void*)&tri, 0, sizeof(md3triangle));
                fileIn.read((char*)&tri, sizeof(md3triangle)); // read the triangles
                lilswap((int *)&tri, 3);
                for (int k = 0; k < 3; k++) m.m_tris[j].vert[k] = (ushort)tri.vertexindices[k];
            }

            m.numverts = mheader.numvertices;
            m.m_uvs.resize(m.numverts);
            fileIn.seekg(mesh_offset + mheader.ofs_uv);
            fileIn.read((char*)m.m_uvs.data(), 2*sizeof(float)*m.numverts); // read the UV data
            lilswap(&m.m_uvs[0].x, 2*m.numverts);

            m.m_verts.resize(numframes*m.numverts + 1);
            fileIn.seekg(mesh_offset + mheader.ofs_vertices);
            for (int j = 0; j < (numframes * mheader.numvertices); j++) {
                md3vertex v = md3vertex();
                // memset((void*)&v, 0, sizeof(md3vertex));
                fileIn.read((char*)&v, sizeof(md3vertex)); // read the vertices
                lilswap((short *)&v, 4);

                m.m_verts[j].x = v.vertex[1] / 64.0f;
                m.m_verts[j].y = v.vertex[0] / 64.0f;
                m.m_verts[j].z = v.vertex[2] / 64.0f;
            }

            mesh_offset += mheader.meshsize;
        }
        fileIn.close();

        filename = newstring(path);
        return true;
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
* MD3 model loader
* ==========
*/
ModelMD3::ModelMD3(const char *name) 
    : Model(name, ModelTypes_t::MDL_MD3) 
{

}

ModelMD3::~ModelMD3() 
{ 

}

void ModelMD3::rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a)
{
    if(parts.length()>1) return;
    parts[0]->rendershadow(anim, varseed, speed, basetime, o, yaw);
}

bool ModelMD3::load()
{
    if(loaded) return true;
    formatstring(md3dir)("packages/models/%s", loadname);

    const char *pname = parentdir(loadname);
    defformatstring(cfgname)("packages/models/%s/md3.cfg", loadname);

    loadingmd3 = this;
    per_idents = false;
    neverpersist = true;
    if(execfile(cfgname) && parts.length()) // configured md3, will call the md3* commands below
    {
        neverpersist = false;
        per_idents = true;
        loadingmd3 = NULL;
        if(parts.empty()) return false;
        loopv(parts) if(!parts[i]->filename) return false;
    }
    else // md3 without configuration, try default tris and skin
    {
        per_idents = false;
        loadingmd3 = NULL;
        MD3Part& mdl = *new MD3Part();
        parts.add(&mdl);
        mdl.model = this;
        mdl.index = 0;
        defformatstring(name1)("packages/models/%s/tris.md3", loadname);
        if(!mdl.load(path(name1)))
        {
            formatstring(name1)("packages/models/%s/tris.md3", pname);    // try md3 in parent folder (vert sharing)
            if(!mdl.load(path(name1))) return false;
        };
        Texture *skin;
        loadskin(loadname, pname, skin);
        for (size_t i = 0; i < mdl.m_meshes.size(); i++) {
            mdl.m_meshes[i]->skin  = skin;
        }
        if(skin==notexture) conoutf("could not load model skin for %s", name1);
    }
    loopv(parts) parts[i]->scaleverts(scale/16.0f, vec3_t(translate.x, -translate.y, translate.z));
    radius = calcradius();
    if(shadowdist) calcneighbors();
    calcbbs();
    return loaded = true;
}

void md3load(char *model)
{
    if(!loadingmd3) { conoutf("not loading an md3"); return; };
    defformatstring(filename)("%s/%s", md3dir, model);
    MD3Part& mdl = *new MD3Part();
    loadingmd3->parts.add(&mdl);
    mdl.model = loadingmd3;
    mdl.index = loadingmd3->parts.length()-1;
    if(!mdl.load(path(filename))) conoutf("could not load %s", filename); // ignore failure
}

void md3skin(char *objname, char *skin)
{
    if(!objname || !skin) return;
    if(!loadingmd3 || loadingmd3->parts.empty()) { conoutf("not loading an md3"); return; };
    ModelPart& mdl = *loadingmd3->parts.last();
    for (size_t i = 0; i < mdl.m_meshes.size(); i++) { 
        Mesh& m = *mdl.m_meshes[i];
        if(!strcmp(objname, "*") || !strcmp(m.name, objname)) {
            defformatstring(spath)("%s/%s", md3dir, skin);
            m.skin = g_TextureMgr.load(spath);
        }
    }
}

void md3anim(char *anim, int *startframe, int *range, float *speed)
{
    if(!loadingmd3 || loadingmd3->parts.empty()) { conoutf("not loading an md3"); return; };
    int num = findanim(anim);
    if(num<0) { conoutf("could not find animation %s", anim); return; };
    loadingmd3->parts.last()->setanim(num, *startframe, *range, *speed);
}

void md3link(int *parent, int *child, char *tagname)
{
    if(!loadingmd3) { conoutf("not loading an md3"); return; };
    if(!loadingmd3->parts.inrange(*parent) || !loadingmd3->parts.inrange(*child)) { conoutf("no models loaded to link"); return; }
    if(!loadingmd3->parts[*parent]->link(loadingmd3->parts[*child], tagname)) conoutf("could not link model %s", loadingmd3->loadname);
}

void md3emit(char *tag, int *type, int *arg1, int *arg2)
{
    if(!loadingmd3 || loadingmd3->parts.empty()) { conoutf("not loading an md3"); return; };
    ModelPart& mdl = *loadingmd3->parts.last();
    if(!mdl.addemitter(tag, *type, *arg1, *arg2)) { conoutf("could not find tag %s", tag); return; }
}

COMMAND(md3load, "s");
COMMAND(md3skin, "ss");
COMMAND(md3anim, "siif");
COMMAND(md3link, "iis");
COMMAND(md3emit, "siii");