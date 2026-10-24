#ifndef RENDERMODEL_H
#define RENDERMODEL_H
#include "cube.h"
#include "texture.h"

void rendermodel(const char *mdl, int anim, int tex, float rad, const vec &o, float yaw, float pitch, float speed = 0, int basetime = 0, playerent *d = NULL, modelattach *a = NULL, float scale = 1.0f);
void startmodelbatches();
void endmodelbatches(bool flush = true);
void clearmodelbatches();
mapmodelinfo &getmminfo(int i);
int findanim(const char *name);
void loadskin(const char *dir, const char *altdir, Texture *&skin);
extern model *nomodel;
model *loadmodel(const char *name, int i = -1, bool trydl = false);
void preload_playermodels();
void preload_entmodels();
void preload_hudguns();
void preload_mapmodels(bool trydl = false);
void renderclients();
void renderclient(playerent *d);
void renderclient(playerent *d, const char *mdlname, const char *vwepname, int tex = 0);
void updateclientname(playerent *d);

#endif