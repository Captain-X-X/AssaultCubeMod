/*
* ==========
* common entity info
* ==========
*/
#ifndef ENTITYINFO_H
#define ENTITYINFO_H
#include "tools/cube_types.h"
#include "itemstat.h"

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

#define isitem(i) ((i) >= I_CLIPS && (i) <= I_AKIMBO)

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

extern itemstat ammostats[NUMGUNS];
extern itemstat powerupstats[I_ARMOUR - I_HEALTH + 1];

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

struct persistent_entity // map entity
{
    short x, y, z; // cube aligned position
    short attr1;
    uchar type; // type is one of the above
    uchar attr2, attr3, attr4;
    persistent_entity(short x, short y, short z, uchar type, short attr1, uchar attr2, uchar attr3, uchar attr4) : x(x), y(y), z(z), attr1(attr1), type(type), attr2(attr2), attr3(attr3), attr4(attr4) {}
    persistent_entity() {}
};

#endif