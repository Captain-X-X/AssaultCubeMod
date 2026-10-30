#ifndef PLAYER_H
#define PLAYER_H
#include "entityinfo.h"
#include "entity.h"
#include "cube.h"

#define MAXNAMELEN 15
#define POSHIST_SIZE 7
#define HEADSIZE 0.4f

class weapon;
class bounceent;
class CBot;

struct poshist
{
    int nextupdate, curpos, numpos;
    vec3_t pos[POSHIST_SIZE];

    poshist();

    const int size() const { return numpos; }

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
    bool allowmove() { return state != CS_DEAD || spectatemode == SM_FLY; }

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

#endif