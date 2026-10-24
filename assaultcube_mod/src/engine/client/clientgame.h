#ifndef CLIENTGAME_H
#define CLIENTGAME_H
#include "cube.h"

extern flaginfo flaginfos[2];
extern int sessionid;
extern int gametimecurrent;
extern int gametimemaximum;
extern int lastgametimeupdate;

struct serverstate
{
    int autoteam;
    int mastermode;
    int matchteamsize;

    void reset()
    {
        autoteam = mastermode = matchteamsize = 0;
    }
};

extern serverstate servstate;

void updateworld(int curtime, int lastmillis);
void resetmap(bool mrproper = true);
void startmap(const char *name, bool reset = true);
void changemap(const char *name);
void initclient();
void deathstate(playerent *pl);
void spawnplayer(playerent *d);
void dodamage(int damage, playerent *pl, playerent *actor, int gun = -1, bool gib = false, bool local = true);
void dokill(playerent *pl, playerent *act, bool gib = false, int gun = 0);
playerent *newplayerent();
botent *newbotent();
void freebotent(botent *d);
char *getclientmap();
int getclientmode();
void zapplayer(playerent *&d);
playerent *getclient(int cn);
playerent *newclient(int cn);
void timeupdate(int milliscur, int millismax); // was (int timeremain);
void respawnself();
void setskin(playerent *pl, int skin, int team = -1);
void callvote(int type, const char *arg1 = NULL, const char *arg2 = NULL, const char *arg3 = NULL);
void addsleep(int msec, const char *cmd, bool persist = false);
void resetsleep(bool force = false);
// game mode extras
void flagpickup(int fln);
void tryflagdrop(bool manual = false);
void flagreturn(int fln);
void flagscore(int fln);
void flagstolen(int flag, int act);
void flagdropped(int flag, float x, float y, float z);
void flaginbase(int flag);
void flagidle(int flag);
void flagmsg(int flag, int message, int actor, int flagtime);
void arenarespawn();
bool tryrespawn();
void findplayerstart(playerent *d, bool mapcenter = false, int arenaspawn = -1);
void serveropcommand(int cmd, int arg1);
void refreshsopmenu(void *menu, bool init);
char *colorname(playerent *d, char *name = NULL, const char *prefix = "");
char *colorping(int ping);
char *colorpj(int pj);
const char *highlight(const char *text);
void togglespect();
playerent *updatefollowplayer(int shiftdirection = 0);
void spectatemode(int mode);

#endif