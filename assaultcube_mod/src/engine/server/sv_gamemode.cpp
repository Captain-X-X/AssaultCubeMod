#include "pch.h"
#include "sv_gamemode.h"

const char *modefullnames[] = {
    "demo playback",
    "team deathmatch", "coopedit", "deathmatch", "survivor",
    "team survivor", "ctf", "pistol frenzy", "bot team deathmatch", "bot deathmatch", "last swiss standing",
    "one shot, one kill", "team one shot, one kill", "bot one shot, one kill", "hunt the flag", "team keep the flag",
    "keep the flag", "team pistol frenzy", "team last swiss standing", "bot pistol frenzy", "bot last swiss standing", "bot team survivor", "bot team one shot, one kill"
};

const char *modeacronymnames[] = {
    "DEMO",
    "TDM", "coop", "DM", "SURV", "TSURV", "CTF", "PF", "BTDM", "BDM", "LSS",
    "OSOK", "TOSOK", "BOSOK", "HTF", "TKTF", "KTF", "TPF", "TLSS", "BPF", "BLSS", "BTSURV", "BTOSOK"
};

const char *fullmodestr(int n) 
{ 
    return (n>=-1 && size_t(n+1) < sizeof(modefullnames)/sizeof(modefullnames[0])) ? modefullnames[n+1] : "unknown"; 
}

const char *acronymmodestr(int n) 
{ 
    return (n>=-1 && size_t(n+1) < sizeof(modeacronymnames)/sizeof(modeacronymnames[0])) ? modeacronymnames[n+1] : "UNK"; 
} // 'n/a' bad on *nix filesystem (demonameformat)

const char *modestr(int n, bool acronyms) 
{ 
    return acronyms ? acronymmodestr (n) : fullmodestr(n); 
}