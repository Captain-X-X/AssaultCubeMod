/*
* ==========
* server side info and handling of the send/recv and playback of demos
* ==========
*/
#ifndef SVDEMO_H
#define SVDEMO_H
#include "pch.h"
#include "tools/filestream.h"
#include "tools/cube_types.h"
#include "tools/cube_vector.h"
#include "tools/cube_string.h"

// demo
extern stream* demotmp;
extern stream* demorecord;
extern stream* demoplayback;
extern bool recordpackets;
extern int nextplayback;

void writedemo(int chan, void *data, int len);
void recordpacket(int chan, void *data, int len);
void recordpacket(int chan, ENetPacket *packet);

#ifdef STANDALONE
const char *currentserver(int i);
#endif

// these are actually the values used by the client, the server ones are in "scl".
extern string demofilenameformat;
extern string demotimestampformat;
extern int demotimelocal;

#ifdef STANDALONE
#define DEMOFORMAT scl.demofilenameformat
#define DEMOTSFORMAT scl.demotimestampformat
#else
#define DEMOFORMAT demofilenameformat
#define DEMOTSFORMAT demotimestampformat
#endif

const char *getDemoFilename(int gmode, int mplay, int mdrop, int tstamp, char *srvmap);
#undef DEMOFORMAT
#undef DEMOTSFORMAT
extern bool sending_demo;
extern int demoprotocol;
extern bool watchingdemo;

void enddemorecord();
void setupdemorecord();
void listdemos(int cn);
static void cleardemos(int n);
void senddemo(int cn, int num);
void enddemoplayback();
void setupdemoplayback();
void readdemo();

#endif