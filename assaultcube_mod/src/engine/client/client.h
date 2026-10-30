#ifndef CLIENT_H
#define CLIENT_H
#include "cube.h"

#include "client/window.h"
#include "client/input.h"
// #include "protocol.h"
#include "server/net_auth.h"

struct clientStatic {
    CWindow window;
    CWindowEventHandler eventHandler;
    CInput input;
};
extern clientStatic g_client;

enum {
    NOT_INITING = 0,
    INIT_LOAD,
    INIT_RESET
};
enum {
    CHANGE_GFX   = 1<<0,
    CHANGE_SOUND = 1<<1
};

/*
* ==========
* 
* ==========
*/
extern int hirestextures;
extern SDL_Surface *screen;
extern int colorbits, depthbits, stencilbits;
extern bool firstrun, inmainloop;
extern bool grabinput, minimized;
extern int initing;
extern bool restoredinits;
extern int fullscreen;
extern int scr_w;
extern int scr_h;
extern int colorbits;
extern int depthbits;
extern int stencilbits;
extern int fsaa;
extern int vsync;
extern int gamestarts;
extern int clockfix;
extern int clockrealbase, clockvirtbase;
extern int clockerror;
extern int paused;
extern int lowfps, highfps;

void cleanup(char *msg);
void quit();
void fatal(const char *s, ...);
bool initwarning(const char *desc, int level = INIT_RESET, int type = CHANGE_GFX);

bool interceptkey(int sym);
void inputgrab(bool on);
void setfullscreen(bool enable);
void screenres(int w, int h);
#if defined(WIN32) || defined(__APPLE__) || !defined(WIN32)
void setresdata(char *s, enet_uint32 c);
#endif
void cleargamma();
void restoregamma();
// void setupscreen(int &usedcolorbits, int &useddepthbits, int &usedfsaa);
const char *rndmapname();
void connectprotocol(char *protocolstring, string &servername, int &serverport, string &password, bool &direct_connect);
void limitfps(int &millis, int curmillis);
void connectserv(char *, int *, char *);

void writeinitcfg();

// client NET stuff
// extern int autodownload;
// void setupcurl();
// bool requirepackage(int type, const char *path);
// int downloadpackages();
// void sortpckservers();
// void writepcksourcecfg();

void abortconnect();
void disconnect(int onlyclean = 0, int async = 0);
void cleanupclient();
void toserver(char *text);
void addmsg(int type, const char *fmt = NULL, ...);
bool multiplayer(bool msg = true);
bool allowedittoggle();
void sendpackettoserv(int chan, ENetPacket *packet);
void gets2c();
void c2sinfo(playerent *d);
void c2skeepalive();
void neterr(const char *s);
int getclientnum();
void changeteam(int team, bool respawn = true); // deprecated?
void getmap(char *name = NULL, char *callback = NULL);
void newteam(char *name);
bool securemapcheck(const char *map, bool msg = true);
int getbuildtype();
void sendintro();
void getdemo(int *idx, char *dsp);
void listdemos();
bool tryauth(const char *desc);
authkey *findauthkey(const char *desc);

#endif