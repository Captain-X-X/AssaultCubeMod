// server.h
#ifndef SERVER_H
#define SERVER_H
#include "pch.h"
#include "cube.h"
#include "protocol.h"

#define gamemode smode   // allows the gamemode macros to work with the server mode

#define SERVER_PROTOCOL_VERSION    (PROTOCOL_VERSION)    // server without any gameplay modification
//#define SERVER_PROTOCOL_VERSION   (-PROTOCOL_VERSION)  // server with gameplay modification but compatible to vanilla client (using /modconnect)
//#define SERVER_PROTOCOL_VERSION  (PROTOCOL_VERSION)    // server with incompatible protocol (change PROTOCOL_VERSION in file protocol.h to a negative number!)

#define valid_flag(f) (f >= 0 && f < 2)

enum { GE_NONE = 0, GE_SHOT, GE_EXPLODE, GE_HIT, GE_AKIMBO, GE_RELOAD, GE_SUICIDE, GE_PICKUP };
enum { ST_EMPTY, ST_LOCAL, ST_TCPIP };

extern int smode, servmillis;

struct shotevent
{
    int type;
    int millis, id;
    int gun;
    float from[3], to[3];
};

struct explodeevent
{
    int type;
    int millis, id;
    int gun;
};

struct hitevent
{
    int type;
    int target;
    int lifesequence;
    union
    {
        int info;
        float dist;
    };
    float dir[3];
};

struct suicideevent
{
    int type;
};

struct pickupevent
{
    int type;
    int ent;
};

struct akimboevent
{
    int type;
    int millis, id;
};

struct reloadevent
{
    int type;
    int millis, id;
    int gun;
};

union gameevent
{
    int type;
    shotevent shot;
    explodeevent explode;
    hitevent hit;
    suicideevent suicide;
    pickupevent pickup;
    akimboevent akimbo;
    reloadevent reload;
};

template <int N>
struct projectilestate
{
    int projs[N];
    int numprojs;

    projectilestate() : numprojs(0) {}

    void reset() { numprojs = 0; }

    void add(int val)
    {
        if(numprojs>=N) numprojs = 0;
        projs[numprojs++] = val;
    }

    bool remove(int val)
    {
        loopi(numprojs) if(projs[i]==val)
        {
            projs[i] = projs[--numprojs];
            return true;
        }
        return false;
    }
};

static const int DEATHMILLIS = 300;

struct clientstate : playerstate
{
    vec o;
    int state;
    int lastdeath, lastspawn, spawn, lifesequence;
    bool forced;
    int lastshot;
    projectilestate<8> grenades;
    int akimbomillis;
    bool scoped;
    int flagscore, frags, teamkills, deaths, shotdamage, damage, points, events, lastdisc, reconnections;

    clientstate() : state(CS_DEAD) {}

    bool isalive(int gamemillis)
    {
        return state==CS_ALIVE || (state==CS_DEAD && gamemillis - lastdeath <= DEATHMILLIS);
    }

    bool waitexpired(int gamemillis)
    {
        int wait = gamemillis - lastshot;
        loopi(NUMGUNS) if(wait < gunwait[i]) return false;
        return true;
    }

    void reset()
    {
        state = CS_DEAD;
        lifesequence = -1;
        grenades.reset();
        akimbomillis = 0;
        scoped = forced = false;
        flagscore = frags = teamkills = deaths = shotdamage = damage = points = events = lastdisc = reconnections = 0;
        respawn();
    }

    void respawn()
    {
        playerstate::respawn();
        o = vec(-1e10f, -1e10f, -1e10f);
        lastdeath = 0;
        lastspawn = -1;
        spawn = 0;
        lastshot = 0;
        akimbomillis = 0;
        scoped = false;
    }
};

struct savedscore
{
    string name;
    uint ip;
    int frags, flagscore, deaths, teamkills, shotdamage, damage, team, points, events, lastdisc, reconnections;
    bool valid, forced;

    void reset()
    {
        // to avoid 2 connections with the same score... this can disrupt some laggers that eventually produces 2 connections (but it is rare)
        frags = flagscore = deaths = teamkills = shotdamage = damage = points = events = lastdisc = reconnections = 0;
    }

    void save(clientstate &cs, int t)
    {
        frags = cs.frags;
        flagscore = cs.flagscore;
        deaths = cs.deaths;
        teamkills = cs.teamkills;
        shotdamage = cs.shotdamage;
        damage = cs.damage;
        points = cs.points;
        forced = cs.forced;
        events = cs.events;
        lastdisc = cs.lastdisc;
        reconnections = cs.reconnections;
        team = t;
        valid = true;
    }

    void restore(clientstate &cs)
    {
        cs.frags = frags;
        cs.flagscore = flagscore;
        cs.deaths = deaths;
        cs.teamkills = teamkills;
        cs.shotdamage = shotdamage;
        cs.damage = damage;
        cs.points = points;
        cs.forced = forced;
        cs.events = events;
        cs.lastdisc = lastdisc;
        cs.reconnections = reconnections;
        reset();
    }
};

struct medals
{
    int dpt, lasthit, lastgun, ncovers, nhs;
    int combohits, combo, combofrags, combotime, combodamage, ncombos;
    int ask, askmillis, linked, linkmillis, linkreason, upmillis, flagmillis;
    int totalhits, totalshots;
    bool updated, combosend;
    vec pos, flagpos;
    void reset()
    {
        dpt = lasthit = lastgun = ncovers = nhs = 0;
        combohits = combo = combofrags = combotime = combodamage = ncombos = 0;
        askmillis = linkmillis = upmillis = flagmillis = 0;
        linkreason = linked = ask = -1;
        totalhits = totalshots = 0;
        updated = combosend = false;
        pos = flagpos = vec(-1e10f, -1e10f, -1e10f);
    }
};

struct client                   // server side version of "dynent" type
{
    int type;
    int clientnum;
    ENetPeer *peer;
    string hostname;
    string name;
    int team;
    char lang[3];
    int ping;
    int skin[2];
    int vote;
    int role;
    int connectmillis, lmillis, ldt, spj;
    int mute, spam, lastvc; // server side voice comm spam control
    int acversion, acbuildtype;
    bool isauthed; // for passworded servers
    bool haswelcome;
    bool isonrightmap, loggedwrongmap, freshgame;
    bool timesync;
    int overflow;
    int gameoffset, lastevent, lastvotecall;
    int lastprofileupdate, fastprofileupdates;
    int demoflags;
    clientstate state;
    vector<gameevent> events;
    vector<uchar> position, messages;
    string lastsaytext;
    int saychars, lastsay, spamcount, badspeech, badmillis;
    int at3_score, at3_lastforce, eff_score;
    bool at3_dontmove;
    int spawnindex;
    int spawnperm, spawnpermsent;
    int salt;
    string pwd;
    uint authreq; // for AUTH
    string authname; // for AUTH
    int mapcollisions, farpickups;
    enet_uint32 bottomRTT;
    medals md;
    bool upspawnp;
    int lag;
    vec spawnp;
    int nvotes;
    int input, inputmillis;
    int ffire, wn, f, g, t, y, p;
    int yb, pb, oy, op, lda, ldda, fam;
    int nt[10], np, lp, ls, lsm, ld, nd, nlt, lem, led;
    vec cp[10], dp[10], d0, lv, lt, le;
    float dda, tr, sda;
    int ps, ph, tcn, bdt, pws;
    float pr;
    int yls, pls, tls;
    int bs, bt, blg, bp;

    gameevent &addevent()
    {
        static gameevent dummy;
        if(events.length()>100) return dummy;
        return events.add();
    }

    void mapchange(bool getmap = false)
    {
        state.reset();
        events.setsize(0);
        overflow = 0;
        timesync = false;
        isonrightmap = m_coop;
        spawnperm = SP_WRONGMAP;
        spawnpermsent = servmillis;
        if(!getmap)
        {
            loggedwrongmap = false;
            freshgame = true;         // permission to spawn at once
        }
        lastevent = 0;
        at3_lastforce = eff_score = 0;
        mapcollisions = farpickups = 0;
        md.reset();
        upspawnp = false;
        lag = 0;
        spawnp = vec(-1e10f, -1e10f, -1e10f);
        lmillis = ldt = spj = 0;
        ffire = 0;
        f = g = y = p = t = 0;
        yb = pb = oy = op = lda = ldda = fam = 0;
        np = lp = ls = lsm = ld = nd = nlt = lem = led = 0;
        d0 = lv = lt = le = vec(0,0,0);
        loopi(10) { cp[i] = dp[i] = vec(0,0,0); nt[i] = 0; }
        dda = tr = sda = 0;
        ps = ph = bdt = pws = 0;
        tcn = -1;
        pr = 0.0f;
        yls = pls = tls = 0;
    }

    void reset()
    {
        name[0] = pwd[0] = demoflags = 0;
        bottomRTT = ping = 9999;
        team = TEAM_SPECT;
        state.state = CS_SPECTATE;
        loopi(2) skin[i] = 0;
        position.setsize(0);
        messages.setsize(0);
        isauthed = haswelcome = false;
        role = CR_DEFAULT;
        lastvotecall = 0;
        lastprofileupdate = fastprofileupdates = 0;
        vote = VOTE_NEUTRAL;
        lastsaytext[0] = '\0';
        saychars = 0;
        spawnindex = -1;
        authreq = 0; // for AUTH
        mapchange();
        freshgame = false;         // don't spawn into running games
        mute = spam = lastvc = badspeech = badmillis = nvotes = 0;
        input = inputmillis = 0;
        wn = -1;
        bs = bt = blg = bp = 0;
    }

    void zap()
    {
        type = ST_EMPTY;
        role = CR_DEFAULT;
        isauthed = haswelcome = false;
    }
};

struct ban
{
    ENetAddress address;
    int millis, type;
};

struct worldstate
{
    enet_uint32 uses;
    vector<uchar> positions, messages;
};

struct server_entity            // server side version of "entity" type
{
    int type;
    bool spawned, legalpickup, twice;
    int spawntime;
    short x, y;
};

struct clientidentity
{
    uint ip;
    int clientnum;
};

struct demofile
{
    string info;
    string file;
    uchar *data;
    int len;
    vector<clientidentity> clientssent;
};

void startgame(const char *newname, int newmode, int newtime = -1, bool notify = true);
void disconnect_client(int n, int reason = -1);
void sendiplist(int receiver, int cn = -1);
int clienthasflag(int cn);
bool refillteams(bool now = false, int ftr = FTR_AUTOTEAM);
void changeclientrole(int client, int role, char *pwd = NULL, bool force=false);
mapstats *getservermapstats(const char *mapname, bool getlayout = false, int *maploc = NULL);
int findmappath(const char *mapname, char *filename = NULL);
int calcscores();
void recordpacket(int chan, void *data, int len);
void senddisconnectedscores(int cn);
void process(ENetPacket *packet, int sender, int chan);
void welcomepacket(packetbuf &p, int n);
void sendwelcome(client *cl, int chan = 1);
void sendpacket(int n, int chan, ENetPacket *packet, int exclude = -1, bool demopacket = false);
int numclients();
bool updateclientteam(int cln, int newteam, int ftr);
void forcedeath(client *cl);
void sendf(int cn, int chan, const char *format, ...);

extern int modeacronyms;
extern void servertoclient(int chan, uchar *buf, int len, bool demo = false);
extern void localservertoclient(int chan, uchar *buf, int len, bool demo = false);
// extern const char *modestr(int n, bool acronyms = false);
extern const char *voteerrorstr(int n);
extern const char *mmfullname(int n);
extern void fatal(const char *s, ...);
extern void initserver(bool dedicated, int argc = 0, char **argv = NULL);
extern void cleanupserver();
extern void localconnect();
extern void localdisconnect();
extern void localclienttoserver(int chan, ENetPacket *);
extern void serverslice(uint timeout);
// extern void putint(ucharbuf &p, int n);
// extern void putint(packetbuf &p, int n);
// extern void putint(vector<uchar> &p, int n);
// extern int getint(ucharbuf &p);
// extern void putuint(ucharbuf &p, int n);
// extern void putuint(packetbuf &p, int n);
// extern void putuint(vector<uchar> &p, int n);
// extern int getuint(ucharbuf &p);
// extern void putfloat(ucharbuf &p, float f);
// extern void putfloat(packetbuf &p, float f);
// extern void putfloat(vector<uchar> &p, float f);
// extern float getfloat(ucharbuf &p);
// extern void sendstring(const char *t, ucharbuf &p);
// extern void sendstring(const char *t, packetbuf &p);
// extern void sendstring(const char *t, vector<uchar> &p);
// extern void getstring(char *t, ucharbuf &p, int len = MAXTRANS);
// extern void filtertext(char *dst, const char *src, int whitespace = 1, int len = sizeof(string)-1);
// extern void filterrichtext(char *dst, const char *src, int len = sizeof(string)-1);
// extern void filterservdesc(char *dst, const char *src, int len = sizeof(string)-1);
// extern void filterlang(char *d, const char *s);
// extern void trimtrailingwhitespace(char *s);
// extern void cutcolorstring(char *text, int len);
extern void startintermission();
extern void restoreserverstate(vector<entity> &ents);
extern string mastername;
extern int masterport;
extern ENetSocket connectmaster();
extern uchar *retrieveservers(uchar *buf, int buflen);
extern void serverms(int mode, int numplayers, int minremain, char *smapname, int millis, const ENetAddress &localaddr, int *mnum, int *msend, int *mrec, int *cnum, int *csend, int *crec, int protocol_version);
extern int msgsizelookup(int msg);
extern const char *genpwdhash(const char *name, const char *pwd, int salt);
extern void servermsinit(const char *master, const char *ip, int serverport, bool listen);
extern bool serverpickup(int i, int sender);
extern bool valid_client(int cn);
extern void extinfo_cnbuf(ucharbuf &p, int cn);
extern void extinfo_statsbuf(ucharbuf &p, int pid, int bpos, ENetSocket &pongsock, ENetAddress &addr, ENetBuffer &buf, int len, int *csend);
extern void extinfo_teamscorebuf(ucharbuf &p);
extern char *votestring(int type, char *arg1, char *arg2, char *arg3);
extern int wizardmain(int argc, char **argv);

extern bool isdedicated;
extern string smapname;
extern mapstats smapstats;
extern char *maplayout;

extern const char *messagenames[SV_NUM];
extern const char *entnames[MAXENTTYPES];
// see entity.h:61: struct itemstat { int add, start, max, sound; };
// Please update ./ac_website/htdocs/docs/introduction.html if these figures change.
extern itemstat ammostats[NUMGUNS];
extern itemstat powerupstats[I_ARMOUR-I_HEALTH+1];
extern guninfo guns[NUMGUNS];
extern const char *teamnames[TEAM_NUM+1];
extern const char *teamnames_s[TEAM_NUM+1];

// for both client and server
// default messages are hardcoded !
extern char killmessages[2][NUMGUNS][MAXKILLMSGLEN];

// demo
#define DHDR_DESCCHARS 80
#define DHDR_PLISTCHARS 322
struct demoheader
{
    char magic[16];
    int version, protocol;
    char desc[DHDR_DESCCHARS];
    char plist[DHDR_PLISTCHARS];
};
#define DEFDEMOFILEFMT "%w_%h_%n_%Mmin_%G"
#define DEFDEMOTIMEFMT "%Y%m%d_%H%M"

extern bool watchingdemo;
extern int demoprotocol;
extern void enddemoplayback();

// server config

struct serverconfigfile
{
    string filename;
    int filelen;
    char *buf;
    serverconfigfile() : filelen(0), buf(NULL) { filename[0] = '\0'; }
    virtual ~serverconfigfile() { DELETEA(buf); }

    virtual void read() {}
    void init(const char *name);
    bool load();
};

// server commandline parsing
struct servercommandline
{
    int uprate, serverport, syslogfacility, filethres, syslogthres, maxdemos, maxclients, kickthreshold, banthreshold, verbose, incoming_limit, afk_limit, ban_time, demotimelocal;
    const char *ip, *master, *logident, *serverpassword, *adminpasswd, *demopath, *maprot, *pwdfile, *blfile, *nbfile, *infopath, *motdpath, *forbidden, *killmessages, *demofilenameformat, *demotimestampformat;
    bool logtimestamp, demo_interm, loggamestatus;
    string motd, servdesc_full, servdesc_pre, servdesc_suf, voteperm, mapperm;
    int clfilenesting;
    vector<const char *> adminonlymaps;

    servercommandline() :   uprate(0), serverport(CUBE_DEFAULT_SERVER_PORT), syslogfacility(6), filethres(-1), syslogthres(-1), maxdemos(5),
                            maxclients(DEFAULTCLIENTS), kickthreshold(-5), banthreshold(-6), verbose(0), incoming_limit(10), afk_limit(45000), ban_time(20*60*1000), demotimelocal(0),
                            ip(""), master(NULL), logident(""), serverpassword(""), adminpasswd(""), demopath(""),
                            maprot("config/maprot.cfg"), pwdfile("config/serverpwd.cfg"), blfile("config/serverblacklist.cfg"), nbfile("config/nicknameblacklist.cfg"),
                            infopath("config/serverinfo"), motdpath("config/motd"), forbidden("config/forbidden.cfg"), killmessages("config/serverkillmessages.cfg"),
                            logtimestamp(false), demo_interm(false), loggamestatus(true),
                            clfilenesting(0)
    {
        motd[0] = servdesc_full[0] = servdesc_pre[0] = servdesc_suf[0] = voteperm[0] = mapperm[0] = '\0';
        demofilenameformat = DEFDEMOFILEFMT;
        demotimestampformat = DEFDEMOTIMEFMT;
    }

    bool checkarg(const char *arg)
    {
        if(!strncmp(arg, "assaultcube://", 13)) return false;
        else if(arg[0] != '-' || arg[1] == '\0') return false;
        const char *a = arg + 2 + strspn(arg + 2, " ");
        int ai = atoi(a);
        // client: dtwhzbsave
        switch(arg[1])
        { // todo: gjlqGHJQUYZ
            case '-':
                    if(!strncmp(arg, "--demofilenameformat=", 21))
                    {
                        demofilenameformat = arg+21;
                    }
                    else if(!strncmp(arg, "--demotimestampformat=", 22))
                    {
                        demotimestampformat = arg+22;
                    }
                    else if(!strncmp(arg, "--demotimelocal=", 16))
                    {
                        int ai = atoi(arg+16);
                        demotimelocal = ai == 0 ? 0 : 1;
                    }
                    else if(!strncmp(arg, "--masterport=", 13))
                    {
                        int ai = atoi(arg+13);
                        masterport = ai == 0 ? AC_MASTER_PORT : ai;
                    }
                    else return false;
                    break;
            case 'u': uprate = ai; break;
            case 'f': if(ai > 0 && ai < 65536) serverport = ai; break;
            case 'i': ip     = a; break;
            case 'm': master = a; break;
            case 'N': logident = a; break;
            case 'l': loggamestatus = ai != 0; break;
            case 'F': if(isdigit(*a) && ai >= 0 && ai <= 7) syslogfacility = ai; break;
            case 'T': logtimestamp = true; break;
            case 'L':
                switch(*a)
                {
                    case 'F': filethres = atoi(a + 1); break;
                    case 'S': syslogthres = atoi(a + 1); break;
                }
                break;
            case 'A': if(*a) adminonlymaps.add(a); break;
            case 'c': if(ai > 0) maxclients = min(ai, MAXCLIENTS); break;
            case 'k':
            {
                if(arg[2]=='A' && arg[3]!='\0')
                {
                    if ((ai = atoi(&arg[3])) >= 30) afk_limit = ai * 1000;
                    else afk_limit = 0;
                }
                else if(arg[2]=='B' && arg[3]!='\0')
                {
                    if ((ai = atoi(&arg[3])) >= 0) ban_time = ai * 60 * 1000;
                    else ban_time = 0;
                }
                else if(ai < 0) kickthreshold = ai;
                break;
            }
            case 'y': if(ai < 0) banthreshold = ai; break;
            case 'x': adminpasswd = a; break;
            case 'p': serverpassword = a; break;
            case 'D':
            {
                if(arg[2]=='I')
                {
                    demo_interm = true;
                }
                else if(ai > 0) maxdemos = ai; break;
            }
            case 'W': demopath = a; break;
            case 'r': maprot = a; break;
            case 'X': pwdfile = a; break;
            case 'B': blfile = a; break;
            case 'K': nbfile = a; break;
            case 'E': killmessages = a; break;
            case 'I': infopath = a; break;
            case 'o': filterrichtext(motd, a); break;
            case 'O': motdpath = a; break;
            case 'g': forbidden = a; break;
            case 'n':
            {
                char *t = servdesc_full;
                switch(*a)
                {
                    case '1': t = servdesc_pre; a += 1 + strspn(a + 1, " "); break;
                    case '2': t = servdesc_suf; a += 1 + strspn(a + 1, " "); break;
                }
                filterrichtext(t, a);
                filterservdesc(t, t);
                break;
            }
            case 'P': concatstring(voteperm, a); break;
            case 'M': concatstring(mapperm, a); break;
            case 'Z': if(ai >= 0) incoming_limit = ai; break;
            case 'V': verbose++; break;
            case 'C': if(*a && clfilenesting < 3)
            {
                serverconfigfile cfg;
                cfg.init(a);
                cfg.load();
                int line = 1;
                clfilenesting++;
                if(cfg.buf)
                {
                    printf("reading commandline parameters from file '%s'\n", a);
                    for(char *p = cfg.buf, *l; p < cfg.buf + cfg.filelen; line++)
                    {
                        l = p; p += strlen(p) + 1;
                        for(char *c = p - 2; c > l; c--) { if(*c == ' ') *c = '\0'; else break; }
                        l += strspn(l, " \t");
                        if(*l && !this->checkarg(newstring(l)))
                            printf("unknown parameter in file '%s', line %d: '%s'\n", cfg.filename, line, l);
                    }
                }
                else printf("failed to read file '%s'\n", a);
                clfilenesting--;
                break;
            }
            default: return false;
        }
        return true;
    }
};

#endif