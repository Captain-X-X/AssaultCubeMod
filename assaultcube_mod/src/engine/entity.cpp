#include "entity.h"
#include "cube.h"
#include "weapon.h"

entity::entity(short x, short y, short z, uchar type, short attr1, uchar attr2, uchar attr3, uchar attr4) 
    : persistent_entity(x, y, z, type, attr1, attr2, attr3, attr4), spawned(false) 
{

}

entity::entity() 
{

}

bool entity::fitsmode(int gamemode) 
{ 
    return !m_noitems && isitem(type) && !(m_noitemsnade && type != I_GRENADE) && !(m_pistol && type == I_AMMO); 
}

void entity::transformtype(int gamemode)
{
    if (m_noitemsnade && type == I_CLIPS)
        type = I_GRENADE;
    else if (m_pistol && (type == I_AMMO || type == I_GRENADE))
        type = I_CLIPS;
}

const char *team_string(int t, bool abbr)
{
    const char **n = abbr ? teamnames_s : teamnames;
    return team_isvalid(t) ? n[t] : n[TEAM_NUM];
}

physent::physent() : o(0, 0, 0), deltapos(0, 0, 0), newpos(0, 0, 0), yaw(270), pitch(0), roll(0), pitchvel(0),
            crouching(false), crouchedinair(false), trycrouch(false), cancollide(true), stuck(false), scoping(false), shoot(false), lastjump(0), lastjumpheight(0), lastsplash(0), state(CS_ALIVE), last_pos(0)
{
    reset();
}
physent::~physent() {}

void physent::resetinterp()
{
    newpos = o;
    newpos.z -= eyeheight;
    deltapos = vec3_t(0, 0, 0);
}

void physent::reset()
{
    vel.x = vel.y = vel.z = eyeheightvel = vel_t.x = vel_t.y = vel_t.z = 0.0f;
    move = strafe = 0;
    timeinair = lastjump = lastsplash = 0;
    onfloor = onladder = inwater = jumpnext = crouching = crouchedinair = trycrouch = stuck = false;
    last_pos = 0;
}

void physent::oncollision() {}
void physent::onmoved(const vec3_t &dist) {}

void dynent::stopmoving()
{
    k_left = k_right = k_up = k_down = jumpnext = false;
    move = strafe = 0;
}

void dynent::resetanim()
{
    loopi(2)
    {
        prev[i].reset();
        current[i].reset();
        lastanimswitchtime[i] = -1;
        lastmodel[i] = NULL;
    }
    lastrendered = 0;
}

void dynent::reset()
{
    physent::reset();
    stopmoving();
}

dynent::dynent()
{
    reset();
    resetanim();
}
dynent::~dynent() {}

// poshist::poshist() : nextupdate(0) { reset(); }

// void poshist::reset()
// {
//     curpos = 0;
//     numpos = 0;
// }

// void poshist::addpos(const vec3_t &o)
// {
//     pos[curpos] = o;
//     curpos++;
//     if (curpos >= POSHIST_SIZE)
//         curpos = 0;
//     if (numpos < POSHIST_SIZE)
//         numpos++;
// }

// const vec3_t& poshist::getpos(int i) const
// {
//     i = curpos - 1 - i;
//     if (i < 0)
//         i += POSHIST_SIZE;
//     return pos[i];
// }

// void poshist::update(const vec3_t &o, int lastmillis)
// {
//     if (lastmillis < nextupdate)
//         return;
//     if (o.dist(pos[0]) >= 4.0f)
//         addpos(o);
//     nextupdate = lastmillis + 100;
// }

// playerstate::playerstate() : armour(0), primary(GUN_ASSAULT), nextprimary(GUN_ASSAULT), akimbo(false) {}
// playerstate::~playerstate() {}

// void playerstate::resetstats() { loopi(NUMGUNS) pstatshots[i] = pstatdamage[i] = 0; }

// itemstat& playerstate::itemstats(int type)
// {
//     switch (type)
//     {
//     case I_CLIPS:
//         return ammostats[GUN_PISTOL];
//     case I_AMMO:
//         return ammostats[primary];
//     case I_GRENADE:
//         return ammostats[GUN_GRENADE];
//     case I_AKIMBO:
//         return ammostats[GUN_AKIMBO];
//     case I_HEALTH:
//     case I_HELMET:
//     case I_ARMOUR:
//         return powerupstats[type - I_HEALTH];
//     default:
//         return *(itemstat *)0;
//     }
// }

// bool playerstate::canpickup(int type)
// {
//     switch (type)
//     {
//     case I_CLIPS:
//         return ammo[akimbo ? GUN_AKIMBO : GUN_PISTOL] < ammostats[akimbo ? GUN_AKIMBO : GUN_PISTOL].max;
//     case I_AMMO:
//         return ammo[primary] < ammostats[primary].max;
//     case I_GRENADE:
//         return mag[GUN_GRENADE] < ammostats[GUN_GRENADE].max;
//     case I_HEALTH:
//         return health < powerupstats[type - I_HEALTH].max;
//     case I_HELMET:
//     case I_ARMOUR:
//         return armour < powerupstats[type - I_HEALTH].max;
//     case I_AKIMBO:
//         return !akimbo;
//     default:
//         return false;
//     }
// }

// void playerstate::additem(itemstat &is, int &v)
// {
//     v += is.add;
//     if (v > is.max)
//         v = is.max;
// }

// void playerstate::pickup(int type)
// {
//     switch (type)
//     {
//     case I_CLIPS:
//         additem(ammostats[GUN_PISTOL], ammo[GUN_PISTOL]);
//         additem(ammostats[GUN_AKIMBO], ammo[GUN_AKIMBO]);
//         break;
//     case I_AMMO:
//         additem(ammostats[primary], ammo[primary]);
//         break;
//     case I_GRENADE:
//         additem(ammostats[GUN_GRENADE], mag[GUN_GRENADE]);
//         break;
//     case I_HEALTH:
//         additem(powerupstats[type - I_HEALTH], health);
//         break;
//     case I_HELMET:
//     case I_ARMOUR:
//         additem(powerupstats[type - I_HEALTH], armour);
//         break;
//     case I_AKIMBO:
//         akimbo = true;
//         mag[GUN_AKIMBO] = guns[GUN_AKIMBO].magsize;
//         additem(ammostats[GUN_AKIMBO], ammo[GUN_AKIMBO]);
//         break;
//     }
// }

// void playerstate::respawn()
// {
//     health = 100;
//     armour = 0;
//     gunselect = GUN_PISTOL;
//     akimbo = false;
//     loopi(NUMGUNS) ammo[i] = mag[i] = gunwait[i] = 0;
//     ammo[GUN_KNIFE] = mag[GUN_KNIFE] = 1;
// }

// void playerstate::spawnstate(int gamemode)
// {
//     if (m_pistol)
//         primary = GUN_PISTOL;
//     else if (m_osok)
//         primary = GUN_SNIPER;
//     else if (m_lss)
//         primary = GUN_KNIFE;
//     else
//         primary = nextprimary;

//     if (!m_nopistol)
//     {
//         ammo[GUN_PISTOL] = ammostats[GUN_PISTOL].start - magsize(GUN_PISTOL); // ammostats[GUN_PISTOL].max-magsize(GUN_PISTOL);
//         mag[GUN_PISTOL] = magsize(GUN_PISTOL);
//     }

//     if (!m_noprimary)
//     {
//         ammo[primary] = ammostats[primary].start - magsize(primary);
//         mag[primary] = magsize(primary);
//     }

//     gunselect = primary;

//     if (m_osok)
//         health = 1;
//     if (m_lms) // Survivor && Team-Survivor : 2010nov19
//     {
//         health = 100;
//         armour = 100;
//         ammo[GUN_GRENADE] = 2;
//     }
// }

// // just subtract damage here, can set death, etc. later in code calling this
// int playerstate::dodamage(int damage, int gun)
// {
//     guninfo gi = guns[gun];
//     if (damage == INT_MAX)
//     {
//         damage = health;
//         armour = health = 0;
//         return damage;
//     }

//     // 4-level armour - tiered approach: 16%, 33%, 37%, 41%
//     // Please update ./ac_website/htdocs/docs/introduction.html if this changes.
//     int armoursection = 0;
//     int ad = damage;
//     if (armour > 25)
//         armoursection = 1;
//     if (armour > 50)
//         armoursection = 2;
//     if (armour > 75)
//         armoursection = 3;
//     switch (armoursection)
//     {
//     case 0:
//         ad = (int)(16.0f / 25.0f * armour);
//         break; // 16
//     case 1:
//         ad = (int)(17.0f / 25.0f * armour) - 1;
//         break; // 33
//     case 2:
//         ad = (int)(4.0f / 25.0f * armour) + 25;
//         break; // 37
//     case 3:
//         ad = (int)(4.0f / 25.0f * armour) + 25;
//         break; // 41
//     default:
//         break;
//     }

//     // ra - reduced armor
//     // rd - reduced damage
//     int ra = (int)(ad * damage / 100.0f);
//     int rd = ra - (ra * (gi.piercing / 100.0f)); // Who cares about rounding errors anyways?

//     armour -= ra;
//     damage -= rd;

//     health -= damage;
//     return damage;
// }

// playerent::playerent() : curskin(0), clientnum(-1), lastupdate(0), plag(0), ping(0), address(0), lifesequence(0), frags(0), flagscore(0), deaths(0), points(0), tks(0), lastpain(0), lastvoicecom(0), clientrole(CR_DEFAULT),
//                 team(TEAM_SPECT), spectatemode(SM_NONE), followplayercn(FPCN_VOID), eardamagemillis(0), respawnoffset(0),
//                 prevweaponsel(NULL), weaponsel(NULL), nextweaponsel(NULL), primweap(NULL), nextprimweap(NULL), lastattackweapon(NULL),
//                 smoothmillis(-1),
//                 head(-1, -1, -1), ignored(false), muted(false)
// {
//     type = ENT_PLAYER;
//     name[0] = 0;
//     maxeyeheight = 4.5f;
//     aboveeye = 0.7f;
//     radius = 1.1f;
//     maxspeed = 16.0f;
//     skin_noteam = skin_cla = skin_rvsf = NULL;
//     loopi(2) nextskin[i] = 0;
//     respawn();
// }

// playerent::~playerent()
// {
//     extern void removebounceents(playerent * owner);
//     extern void removedynlights(physent * owner);
//     extern void zapplayerflags(playerent * owner);
//     extern void cleanplayervotes(playerent * owner);
//     extern physent *camera1;
//     extern void togglespect();
//     removebounceents(this);
//     audiomgr.detachsounds(this);
//     removedynlights(this);
//     zapplayerflags(this);
//     cleanplayervotes(this);
//     if (this == camera1)
//         togglespect();
// }

// void playerent::damageroll(float damage)
// {
//     extern void clamproll(physent * pl);
//     float damroll = 2.0f * damage;
//     roll += roll > 0 ? damroll : (roll < 0 ? -damroll : (rnd(2) ? damroll : -damroll)); // give player a kick
//     clamproll(this);
// }

// void playerent::hitpush(int damage, const vec3_t &dir, playerent *actor, int gun)
// {
//     if (gun < 0 || gun > NUMGUNS)
//         return;
//     vec3_t push(dir);
//     push.mul(damage / 100.0f * guns[gun].pushfactor);
//     vel.add(push);
//     extern int lastmillis;
//     if (gun == GUN_GRENADE && damage > 50)
//         eardamagemillis = lastmillis + damage * 100;
// }

// void playerent::resetspec()
// {
//     spectatemode = SM_NONE;
//     followplayercn = FPCN_VOID;
// }

// void playerent::respawn()
// {
//     dynent::reset();
//     playerstate::respawn();
//     history.reset();
//     if (weaponsel)
//         weaponsel->reset();
//     lastaction = 0;
//     lastattackweapon = NULL;
//     attacking = false;
//     extern int lastmillis;
//     weaponchanging = lastmillis - weapons[gunselect]->weaponchangetime / 2; // 2011jan16:ft: for a little no-shoot after spawn
//     resetspec();
//     eardamagemillis = 0;
//     eyeheight = maxeyeheight;
//     curskin = nextskin[team_base(team)];
// }

// void playerent::spawnstate(int gamemode)
// {
//     playerstate::spawnstate(gamemode);
//     prevweaponsel = weaponsel = weapons[gunselect];
//     primweap = weapons[primary];
//     nextprimweap = weapons[nextprimary];
//     curskin = nextskin[team_base(team)];
// }

// void playerent::selectweapon(int w)
// {
//     if (weaponsel)
//         prevweaponsel = weaponsel;
//     weaponsel = weapons[(gunselect = w)];
//     if (!prevweaponsel)
//         prevweaponsel = weaponsel;
// }
// void playerent::setprimary(int w) { primweap = weapons[(primary = w)]; }
// void playerent::setnextprimary(int w) { nextprimweap = weapons[(nextprimary = w)]; }
// bool playerent::isspectating() { return state == CS_SPECTATE || (state == CS_DEAD && spectatemode > SM_NONE); }
// void playerent::weaponswitch(weapon *w)
// {
//     if (!w)
//         return;
//     extern int lastmillis;
//     weaponsel->ondeselecting();
//     weaponchanging = lastmillis;
//     nextweaponsel = w;
//     w->onselecting();
// }
// int playerent::skin(int t) { return nextskin[team_base(t < 0 ? team : t)]; }
// void playerent::setskin(int t, int s)
// {
//     const int maxskin[2] = {4, 6};
//     t = team_base(t < 0 ? team : t);
//     nextskin[t] = abs(s) % maxskin[t];
// }

bounceent::bounceent() : bouncetype(BT_NONE), rotspeed(1.0f), plclipped(false), owner(NULL)
{
    type = ENT_BOUNCE;
    maxspeed = 40;
    radius = 0.2f;
    eyeheight = maxeyeheight = 0.3f;
    aboveeye = 0.0f;
}
bool bounceent::isalive(int lastmillis) { return lastmillis - millis < timetolive; }
