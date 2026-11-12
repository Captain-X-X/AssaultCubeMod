#ifndef ENTITY_H
#define ENTITY_H
// #include "cube.h"
#include "tools/cube_types.h"
#include "server/protocol.h"

enum // static entity types
{
    NOTUSED = 0, // entity slot not in use in map
    LIGHT,       // lightsource, attr1 = radius, attr2 = intensity
    PLAYERSTART, // attr1 = angle, attr2 = team
    I_CLIPS,
    I_AMMO,
    I_GRENADE,
    I_HEALTH,
    I_HELMET,
    I_ARMOUR,
    I_AKIMBO,
    // helmet : 2010may16 -> mapversion:8
    MAPMODEL, // attr1 = angle, attr2 = idx
    CARROT,   // attr1 = tag, attr2 = type
    LADDER,
    CTF_FLAG, // attr1 = angle, attr2 = red/blue
    SOUND,
    CLIP,
    PLCLIP,
    MAXENTTYPES
};

enum
{
    MAP_IS_BAD,
    MAP_IS_EDITABLE,
    MAP_IS_GOOD
};

extern const char *entnames[MAXENTTYPES];
#define isitem(i) ((i) >= I_CLIPS && (i) <= I_AKIMBO)

struct persistent_entity // map entity
{
    short x, y, z; // cube aligned position
    short attr1;
    uchar type; // type is one of the above
    uchar attr2, attr3, attr4;
    persistent_entity(short x, short y, short z, uchar type, short attr1, uchar attr2, uchar attr3, uchar attr4);
    persistent_entity();
};

struct entity : persistent_entity
{
    bool spawned; // the dynamic states of a map entity
    int lastmillis;
    entity(short x, short y, short z, uchar type, short attr1, uchar attr2, uchar attr3, uchar attr4);
    entity();
    bool fitsmode(int gamemode);
    void transformtype(int gamemode);
};

enum
{
    GUN_KNIFE = 0,
    GUN_PISTOL,
    GUN_CARBINE,
    GUN_SHOTGUN,
    GUN_SUBGUN,
    GUN_SNIPER,
    GUN_ASSAULT,
    GUN_CPISTOL,
    GUN_GRENADE,
    GUN_AKIMBO,
    NUMGUNS
};
#define reloadable_gun(g) (g != GUN_KNIFE && g != GUN_GRENADE)

#define SGRAYS 21
#define SGDMGTOTAL 90

#define SGDMGBONUS 65
#define SGDMGDISTB 50

#define SGCCdmg 500
#define SGCCbase 0
#define SGCCrange 40

#define SGCMdmg 375
#define SGCMbase 25
#define SGCMrange 60

#define SGCOdmg 125
#define SGCObase 45
#define SGCOrange 75

#define SGMAXDMGABS 105
#define SGMAXDMGLOC 84
#define SGBONUSDIST 80
#define SGSEGDMG_O 3
#define SGSEGDMG_M 6
#define SGSEGDMG_C 4
#define SGSPREAD 2.25
#define EXPDAMRAD 10

struct itemstat
{
    int add, start, max, sound;
};
extern itemstat ammostats[NUMGUNS];
extern itemstat powerupstats[I_ARMOUR - I_HEALTH + 1];

struct guninfo
{
    string modelname;
    short sound;
    short reload, reloadtime;
    short attackdelay, damage, piercing, projspeed, part, spread, recoil;
    short magsize, mdl_kick_rot, mdl_kick_back, recoilincrease, recoilbase, maxrecoil, recoilbackfade;
    short pushfactor;
    bool isauto;
};
extern guninfo guns[NUMGUNS];

static inline int reloadtime(int gun) { return guns[gun].reloadtime; }
static inline int attackdelay(int gun) { return guns[gun].attackdelay; }
static inline int magsize(int gun) { return guns[gun].magsize; }

/** roseta stone:
       0000,         0001,      0010,           0011,            0100,       0101,     0110 */
enum
{
    TEAM_CLA = 0,
    TEAM_RVSF,
    TEAM_CLA_SPECT,
    TEAM_RVSF_SPECT,
    TEAM_SPECT,
    TEAM_NUM,
    TEAM_ANYACTIVE
};
extern const char *teamnames[TEAM_NUM + 1];
extern const char *teamnames_s[TEAM_NUM + 1];

#define TEAM_VOID TEAM_NUM
#define isteam(a, b) (m_teammode && (a) == (b))
#define team_opposite(o) (team_isvalid(o) && (o) < TEAM_SPECT ? (o) ^ 1 : TEAM_SPECT)
#define team_base(t) ((t) & 1)
#define team_basestring(t) ((t) == 1 ? teamnames[1] : ((t) == 0 ? teamnames[0] : "SPECT"))
#define team_isvalid(t) ((int(t)) >= 0 && (t) < TEAM_NUM)
#define team_isactive(t) ((t) == TEAM_CLA || (t) == TEAM_RVSF)
#define team_isspect(t) ((t) > TEAM_RVSF && (t) < TEAM_VOID)
#define team_group(t) ((t) == TEAM_SPECT ? TEAM_SPECT : team_base(t))
#define team_tospec(t) ((t) == TEAM_SPECT ? TEAM_SPECT : team_base(t) + TEAM_CLA_SPECT - TEAM_CLA)
// note: team_isactive and team_base can/should be used to check the limits for arrays of size '2'
static inline const char *team_string(int t, bool abbr = false)
{
    const char **n = abbr ? teamnames_s : teamnames;
    return team_isvalid(t) ? n[t] : n[TEAM_NUM];
}

enum
{
    ENT_PLAYER = 0,
    ENT_BOT,
    ENT_CAMERA,
    ENT_BOUNCE
};
enum
{
    CS_ALIVE = 0,
    CS_DEAD,
    CS_SPAWNING,
    CS_LAGGED,
    CS_EDITING,
    CS_SPECTATE
};
enum
{
    CR_DEFAULT = 0,
    CR_ADMIN
};
enum
{
    SM_NONE = 0,
    SM_DEATHCAM,
    SM_FOLLOW1ST,
    SM_FOLLOW3RD,
    SM_FOLLOW3RD_TRANSPARENT,
    SM_FLY,
    SM_OVERVIEW,
    SM_NUM
};
enum
{
    FPCN_VOID = -4,
    FPCN_DEATHCAM = -2,
    FPCN_FLY = -2,
    FPCN_OVERVIEW = -1
};

class worldobject
{
public:
    virtual ~worldobject() {};
};

class physent : public worldobject
{
public:
    vec3_t o, vel, vel_t;      // origin, velocity
    vec3_t deltapos, newpos;   // movement interpolation
    float yaw, pitch, roll; // used as vec3_t in one place
    float pitchvel;
    float maxspeed;                                  // cubes per second, 24 for player
    int timeinair;                                   // used for fake gravity
    float radius, eyeheight, maxeyeheight, aboveeye; // bounding box size
    bool inwater;
    bool onfloor, onladder, jumpnext, crouching, crouchedinair, trycrouch, cancollide, stuck, scoping, shoot;
    int lastjump;
    float lastjumpheight;
    int lastsplash;
    char move, strafe;
    uchar state, type;
    float eyeheightvel;
    int last_pos;

    physent();
    virtual ~physent();
    
    void resetinterp();
    void reset();
    virtual void oncollision() {}
    virtual void onmoved(const vec3_t &dist) {}
};

class dynent : public physent // animated ent
{
public:
    bool k_left, k_right, k_up, k_down; // see input code

    animstate prev[2], current[2]; // md2's need only [0], md3's need both for the lower&upper model
    int lastanimswitchtime[2];
    void *lastmodel[2];
    int lastrendered;

public:
    dynent();
    virtual ~dynent();
  
    void stopmoving();
    void resetanim();
    void reset();
};

#define MAXNAMELEN 15

class bounceent;

#define POSHIST_SIZE 7

struct poshist
{
    int nextupdate, curpos, numpos;
    vec3_t pos[POSHIST_SIZE];

    poshist();

    const int size() const;
    void reset();
    void addpos(const vec3_t &o);
    const vec3_t &getpos(int i) const;
    void update(const vec3_t &o, int lastmillis);
};

class playerstate
{
public:
    int health, armour;
    int primary, nextprimary;
    int gunselect;
    bool akimbo;
    int ammo[NUMGUNS], mag[NUMGUNS], gunwait[NUMGUNS];
    int pstatshots[NUMGUNS], pstatdamage[NUMGUNS];

    playerstate();
    virtual ~playerstate();

    void resetstats();
    itemstat &itemstats(int type);
    bool canpickup(int type);
    void additem(itemstat &is, int &v);
    void pickup(int type);
    void respawn();
    virtual void spawnstate(int gamemode);

    // just subtract damage here, can set death, etc. later in code calling this
    int dodamage(int damage, int gun);
};

#ifndef STANDALONE
#define HEADSIZE 0.4f

class playerent : public dynent, public playerstate
{
private:
    int curskin, nextskin[2];

public:
    int clientnum, lastupdate, plag, ping;
    enet_uint32 address;
    int lifesequence; // sequence id for each respawn, used in damage test
    int frags, flagscore, deaths, points, tks;
    int lastaction, lastmove, lastpain, lastvoicecom;
    int clientrole;
    bool attacking;
    string name;
    int team;
    int weaponchanging;
    int nextweapon; // weapon we switch to
    int spectatemode, followplayercn;
    int eardamagemillis;
    int respawnoffset;
    bool allowmove();

    weapon *weapons[NUMGUNS];
    weapon *prevweaponsel, *weaponsel, *nextweaponsel, *primweap, *nextprimweap, *lastattackweapon;

    poshist history; // Previous stored locations of this player

    const char *skin_noteam, *skin_cla, *skin_rvsf;

    float deltayaw, deltapitch, newyaw, newpitch;
    int smoothmillis;

    vec3_t head;

    bool ignored, muted;

    playerent();
    virtual ~playerent();
    
    void damageroll(float damage);
    void hitpush(int damage, const vec3_t &dir, playerent *actor, int gun);
    void resetspec();
    void respawn();
    void spawnstate(int gamemode);
    void selectweapon(int w);
    void setprimary(int w);
    void setnextprimary(int w);
    bool isspectating();
    void weaponswitch(weapon *w);
    int skin(int t = -1);
    void setskin(int t, int s);
};

class CBot;

class botent : public playerent
{
public:
    // Added by Rick
    CBot *pBot; // Only used if this is a bot, points to the bot class if we are the host,
                // for other clients its NULL
    // End add by Rick

    playerent *enemy; // monster wants to kill this entity
    // Added by Rick: targetpitch
    float targetpitch; // monster wants to look in this direction
    // End add
    float targetyaw; // monster wants to look in this direction

    botent() : pBot(NULL), enemy(NULL) { type = ENT_BOT; }
    ~botent() {}

    int deaths() { return lifesequence; }
};
#endif // #ifndef STANDALONE

// flag-mode entities
enum
{
    CTFF_INBASE = 0,
    CTFF_STOLEN,
    CTFF_DROPPED,
    CTFF_IDLE
};

struct flaginfo
{
    int team;
    entity *flagent;
    int actor_cn;
    playerent *actor;
    vec3_t pos;
    int state; // one of CTFF_*
    bool ack;
    flaginfo() : flagent(0), actor(0), state(CTFF_INBASE), ack(false) {}
};

// nades, gibs
enum
{
    BT_NONE,
    BT_NADE,
    BT_GIB
};

class bounceent : public physent
{
public:
    int millis, timetolive, bouncetype; // see enum above
    float rotspeed;
    bool plclipped;
    playerent *owner;

    bounceent();
    virtual ~bounceent();

    bool isalive(int lastmillis);
    virtual void destroy() {}
    virtual bool applyphysics() { return true; }
};

struct hitmsg
{
    int target, lifesequence, info;
    ivec dir;
};

class grenadeent : public bounceent
{
public:
    bool local;
    int nadestate;
    float distsincebounce;
    grenadeent(playerent *owner, int millis = 0);
    ~grenadeent();
    void activate(const vec3_t &from, const vec3_t &to);
    void _throw(const vec3_t &from, const vec3_t &vel);
    void explode();
    void splash();
    virtual void destroy();
    virtual bool applyphysics();
    void moveoutsidebbox(const vec3_t &direction, playerent *boundingbox);
    void oncollision();
    void onmoved(const vec3_t &dist);
};

enum
{
    MD_FRAGS = 0,
    MD_DEATHS,
    END_MDS
};
struct medalsst
{
    bool assigned;
    int cn;
    int item;
};

#define MAXKILLMSGLEN 16
extern char killmessages[2][NUMGUNS][MAXKILLMSGLEN];
inline const char *killmessage(int gun, bool gib = false)
{
    if (gun < 0 || gun >= NUMGUNS)
        return "";

    return killmessages[gib ? 1 : 0][gun];
}

#ifndef STANDALONE
struct pckserver
{
    char *addr;
    bool pending, responsive;
    int ping;

    pckserver() : addr(NULL), pending(false), responsive(true), ping(-1) {}
};

enum
{
    PCK_TEXTURE,
    PCK_SKYBOX,
    PCK_MAPMODEL,
    PCK_AUDIO,
    PCK_MAP,
    PCK_NUM
};

struct package
{
    char *name;
    int type, number;
    bool pending;
    pckserver *source;
    CURL *curl;

    package() : name(NULL), type(-1), number(0), pending(false), source(NULL), curl(NULL) {}
};
#endif
#endif