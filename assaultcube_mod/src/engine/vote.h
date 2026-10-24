#ifndef GVOTE_H
#define GVOTE_H
#include "cube.h"

struct votedisplayinfo
{
    playerent *owner;
    int type, stats[VOTE_NUM], result, millis;
    string desc;
    bool localplayervoted;
    votedisplayinfo() 
        : owner(NULL), result(VOTE_NEUTRAL), millis(0), localplayervoted(false) 
    { 
        for (int i = 0; i < VOTE_NUM; i++) stats[i] = VOTE_NEUTRAL; 
    }
};

votedisplayinfo *newvotedisplayinfo(playerent *owner, int type, const char *arg1, const char *arg2, const char *arg3 = "");
void callvotesuc();
void callvoteerr(int e);
void displayvote(votedisplayinfo *v);
void voteresult(int v);
void votecount(int v);
void clearvote();

#endif