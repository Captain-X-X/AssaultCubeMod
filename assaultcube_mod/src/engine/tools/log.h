#ifndef LOG_H
#define LOG_H

enum
{
    ACLOG_DEBUG = 0,
    ACLOG_VERBOSE,
    ACLOG_INFO,
    ACLOG_WARNING,
    ACLOG_ERROR,
    ACLOG_NUM
};

bool initlogging(const char *identity, int facility_, int consolethres, int filethres, int syslogthres, bool logtimestamp);
void exitlogging();
bool logline(int level, const char *msg, ...);

#endif