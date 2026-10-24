/*
* ==========
* server side game mode info
* ==========
*/
#ifndef SVGAMEMODE_H
#define SVGAMEMODE_H

extern const char *modefullnames[];
extern const char *modeacronymnames[];

const char *fullmodestr(int n);
const char *acronymmodestr(int n);
const char *modestr(int n, bool acronyms = true);

#endif