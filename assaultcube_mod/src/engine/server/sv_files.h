#ifndef SERVERFILES_H
#define SERVERFILES_H
#include "cube.h"
#include "lib/enet/include/enet/enet.h"

struct client;

// serverfiles.h
// map management

#define SERVERMAP_PATH          "packages/maps/servermaps/"
#define SERVERMAP_PATH_BUILTIN  "packages/maps/official/"
#define SERVERMAP_PATH_INCOMING "packages/maps/servermaps/incoming/"
#define GZBUFSIZE ((MAXCFGFILESIZE * 11) / 10)

struct servermapbuffer  // sending of maps between clients
{
    string mapname;
    int cgzsize, cfgsize, cfgsizegz, revision, datasize;
    uchar *data, *gzbuf;

    servermapbuffer();
    ~servermapbuffer();

    void clear();
    int available();
    void setrevision();
    void load(void);  // load currently played map into the buffer (if distributable), clear buffer otherwise
    bool sendmap(const char *nmapname, int nmapsize, int ncfgsize, int ncfgsizegz, uchar *ndata);
    void sendmap(client *cl, int chan);
};

// provide maps by the server

enum { MAP_NOTFOUND = 0, MAP_TEMP, MAP_CUSTOM, MAP_LOCAL, MAP_OFFICIAL, MAP_VOID };
static const char * const maplocstr[] = { "not found", "temporary", "custom", "local", "official", "void" };
#define readonlymap(x) ((x) >= MAP_CUSTOM)
#define distributablemap(x) ((x) == MAP_TEMP || (x) == MAP_CUSTOM)

int findmappath(const char *mapname, char *filename);
mapstats *getservermapstats(const char *mapname, bool getlayout, int *maploc);

// maprot.cfg
#define CONFIG_MAXPAR 6

struct configset
{
    string mapname;
    union
    {
        struct { int mode, time, vote, minplayer, maxplayer, skiplines; };
        int par[CONFIG_MAXPAR];
    };
};

extern int FlagFlag;
extern int Mvolume, Marea, SHhits, Mopen;
extern float Mheight;

bool mapisok(mapstats *ms);

struct servermaprot : serverconfigfile
{
    vector<configset> configsets;
    int curcfgset;

    servermaprot();

    void read();
    int next(bool notify = true, bool nochange = false); // load next maprotation set
    void restart(bool notify = true); // restart current map
    configset *current();
    configset *get(int ccs);
    int get_next();
};

// serverblacklist.cfg

struct serveripblacklist : serverconfigfile
{
    vector<iprange> ipranges;

    void read();
    bool check(enet_uint32 ip); // ip: network byte order
};
// nicknameblacklist.cfg

#define MAXNICKFRAGMENTS 5
enum { NWL_UNLISTED = 0, NWL_PASS, NWL_PWDFAIL, NWL_IPFAIL };

struct servernickblacklist : serverconfigfile
{
    struct iprchain     { struct iprange ipr; const char *pwd; int next; };
    struct blackline    { int frag[MAXNICKFRAGMENTS]; bool ignorecase; int line; void clear() { loopi(MAXNICKFRAGMENTS) frag[i] = -1; } };
    hashtable<const char *, int> whitelist;
    vector<iprchain> whitelistranges;
    vector<blackline> blacklines;
    vector<const char *> blfraglist;

    void destroylists();
    void read();
    int checkwhitelist(const client &c);
    int checkblacklist(const char *name);
};

#define FORBIDDENSIZE 15
struct serverforbiddenlist : serverconfigfile
{
    int num;
    char entries[100][2][FORBIDDENSIZE+1]; // 100 entries and 2 words per entry is more than enough

    void initlist();
    void addentry(char *s);
    void read();
    bool canspeech(char *s);
};

// serverpwd.cfg

#define ADMINPWD_MAXPAR 1
struct pwddetail
{
    string pwd;
    int line;
    bool denyadmin;    // true: connect only
};

extern int passtime, passtries;
extern enet_uint32 passguy;

struct serverpasswords : serverconfigfile
{
    vector<pwddetail> adminpwds;
    int staticpasses;

    serverpasswords();

    void init(const char *name, const char *cmdlinepass);
    void read();
    bool check(const char *name, const char *pwd, int salt, pwddetail *detail = NULL, enet_uint32 address = 0);
};

// serverinfo_en.txt, motd_en.txt

#define MAXINFOLINELEN 100  // including color codes

struct serverinfofile
{
    struct serverinfotext { const char *type; char lang[3]; char *info; int lastcheck; };
    vector<serverinfotext> serverinfotexts;
    const char *infobase, *motdbase;

    void init(const char *info, const char *motd) { }
    char *readinfofile(const char *fnbase, const char *lang);
    const char *getinfocache(const char *fnbase, const char *lang);
    const char *getinfo(const char *lang);
    const char *getmotd(const char *lang);

};

struct killmessagesfile : serverconfigfile
{
    void init(const char *name);
    void read();
};

#endif