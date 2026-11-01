// protos for ALL external functions in cube...
#ifndef PROTO_H
#define PROTO_H

#include "tools/cube_vector.h"
#include "tools/cube_hashtable.h"
#include "tools/cube_string.h"
#include "tools/filestream.h"
#include "tools/cube_types.h"
#include "tools/tools.h"

using namespace Maths;

#include "renderer/rendergl.h"

struct authkey // for AUTH
{
    char *name, *key, *desc;
    int lastauth;

    authkey(const char *name, const char *key, const char *desc)
        : name(newstring(name)), key(newstring(key)), desc(newstring(desc)),
          lastauth(0)
    {
    }

    ~authkey()
    {
        DELETEA(name);
        DELETEA(key);
        DELETEA(desc);
    }
};

// serverbrowser
extern void addserver(const char *servername, int serverport, int weight);
extern char *getservername(int n);
extern bool resolverwait(const char *name, ENetAddress *address);
extern int connectwithtimeout(ENetSocket sock, const char *hostname, ENetAddress &remoteaddress);
extern void writeservercfg();
extern void refreshservers(void *menu, bool init);
extern bool serverskey(void *menu, int code, bool isdown, int unicode);
extern bool serverinfokey(void *menu, int code, bool isdown, int unicode);

struct serverinfo
{
    enum { UNRESOLVED = 0, RESOLVING, RESOLVED };

    string name;
    string full;
    string map;
    string sdesc;
    string description;
    string cmd;
    int mode, numplayers, maxclients, ping, protocol, minremain, resolved, port, lastpingmillis, pongflags, getinfo, menuline_from, menuline_to;
    ENetAddress address;
    vector<const char *> playernames;
    uchar namedata[MAXTRANS];
    vector<char *> infotexts;
    uchar textdata[MAXTRANS];
    char lang[3];
    color *bgcolor;
    int favcat, msweight, weight;

    int uplinkqual, uplinkqual_age;
    unsigned char uplinkstats[MAXCLIENTS + 1];

    serverinfo()
     : mode(0), numplayers(0), maxclients(0), ping(9999), protocol(0), minremain(0),
       resolved(UNRESOLVED), port(-1), lastpingmillis(0), pongflags(0), getinfo(EXTPING_NOP),
       bgcolor(NULL), favcat(-1), msweight(0), weight(0), uplinkqual(0), uplinkqual_age(0)
    {
        name[0] = full[0] = map[0] = sdesc[0] = description[0] = '\0';
        loopi(3) lang[i] = '\0';
        loopi(MAXCLIENTS + 1) uplinkstats[i] = 0;
    }
};

extern serverinfo *getconnectedserverinfo();
extern void pingservers();
extern void updatefrommaster(int force);

struct packetqueue
{
    ringbuf<ENetPacket *, 8> packets;

    packetqueue();
    ~packetqueue();
    void queue(ENetPacket *p);
    bool flushtolog(const char *logfile);
    void clear();
};

// client
extern void abortconnect();
extern void disconnect(int onlyclean = 0, int async = 0);
extern void cleanupclient();
extern void toserver(char *text);
extern void addmsg(int type, const char *fmt = NULL, ...);
extern bool multiplayer(bool msg = true);
extern bool allowedittoggle();
extern void sendpackettoserv(int chan, ENetPacket *packet);
extern void gets2c();
extern void c2sinfo(playerent *d);
extern void c2skeepalive();
extern void neterr(const char *s);
extern int getclientnum();
extern void changeteam(int team, bool respawn = true); // deprecated?
extern void getmap(char *name = NULL, char *callback = NULL);
extern void newteam(char *name);
extern bool securemapcheck(const char *map, bool msg = true);
extern int getbuildtype();
extern void sendintro();
extern void getdemo(int *idx, char *dsp);
extern void listdemos();
extern bool tryauth(const char *desc);
extern authkey *findauthkey(const char *desc);

// serverms
bool requestmasterf(const char *fmt, ...); // for AUTH et al
//moving to server.cpp seems a bad idea.
// :for AUTH

// weapon
extern void shoot(playerent *d, vec &to);
extern void createrays(vec &from, vec &to);
extern void removebounceents(playerent *owner);
extern void movebounceents();
extern void clearbounceents();
extern void renderbounceents();
extern void addgib(playerent *d);
extern playerent *playerincrosshair();
extern int magsize(int gun);
extern void setscope(bool activate);
extern void setburst(bool activate);
extern int intersect(playerent *d, const vec &from, const vec &to, vec *end = NULL);
extern bool intersect(entity *e, const vec &from, const vec &to, vec *end = NULL);
// Structure for storing traceresults
struct traceresult_s
{
     vec end;
     bool collided;
};
void TraceLine(vec from, vec to, dynent *pTracer, bool CheckPlayers, traceresult_s *tr, bool SkipTags=false);
extern void damageeffect(int damage, playerent *d);
extern void tryreload(playerent *p);
extern void checkweaponstate();
extern struct projectile *newprojectile(vec &from, vec &to, float speed, bool local, playerent *owner, int gun, int id = lastmillis);
extern int burstshotssettings[NUMGUNS];

// protocol [client and server]
extern const char *acronymmodestr(int n);
extern const char *fullmodestr(int n);
extern int defaultgamelimit(int gamemode);

// crypto // for AUTH
extern void genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr);
extern bool hashstring(const char *str, char *result, int maxlen);
const char *genpwdhash(const char *name, const char *pwd, int salt);
extern void answerchallenge(const char *privstr, const char *challenge, vector<char> &answerstr);
extern void *parsepubkey(const char *pubstr);
extern void freepubkey(void *pubkey);
extern void *genchallenge(void *pubkey, const void *seed, int seedlen, vector<char> &challengestr);
extern void freechallenge(void *answer);
extern bool checkchallenge(const char *answerstr, void *correct);

// console
extern void conoutf(const char *s, ...);

// server
extern int modeacronyms;
extern void servertoclient(int chan, uchar *buf, int len, bool demo = false);
extern void localservertoclient(int chan, uchar *buf, int len, bool demo = false);
extern const char *modestr(int n, bool acronyms = false);
extern const char *voteerrorstr(int n);
extern const char *mmfullname(int n);
extern void fatal(const char *s, ...);
extern void initserver(bool dedicated, int argc = 0, char **argv = NULL);
extern void cleanupserver();
extern void localconnect();
extern void localdisconnect();
extern void localclienttoserver(int chan, ENetPacket *);
extern void serverslice(uint timeout);
extern void putint(ucharbuf &p, int n);
extern void putint(packetbuf &p, int n);
extern void putint(vector<uchar> &p, int n);
extern int getint(ucharbuf &p);
extern void putuint(ucharbuf &p, int n);
extern void putuint(packetbuf &p, int n);
extern void putuint(vector<uchar> &p, int n);
extern int getuint(ucharbuf &p);
extern void putfloat(ucharbuf &p, float f);
extern void putfloat(packetbuf &p, float f);
extern void putfloat(vector<uchar> &p, float f);
extern float getfloat(ucharbuf &p);
extern void sendstring(const char *t, ucharbuf &p);
extern void sendstring(const char *t, packetbuf &p);
extern void sendstring(const char *t, vector<uchar> &p);
extern void getstring(char *t, ucharbuf &p, int len = MAXTRANS);
extern void filtertext(char *dst, const char *src, int whitespace = 1, int len = sizeof(string)-1);
extern void filterrichtext(char *dst, const char *src, int len = sizeof(string)-1);
extern void filterservdesc(char *dst, const char *src, int len = sizeof(string)-1);
extern void filterlang(char *d, const char *s);
extern void trimtrailingwhitespace(char *s);
extern void cutcolorstring(char *text, int len);
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

// logging

enum { ACLOG_DEBUG = 0, ACLOG_VERBOSE, ACLOG_INFO, ACLOG_WARNING, ACLOG_ERROR, ACLOG_NUM };

extern bool initlogging(const char *identity, int facility_, int consolethres, int filethres, int syslogthres, bool logtimestamp);
extern void exitlogging();
extern bool logline(int level, const char *msg, ...);

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

// shotty: shotgun rays def
struct sgray {
    int ds; // damage flag: outer, medium, center: SGSEGDMG_*
    vec rv; // ray vector
};

#endif