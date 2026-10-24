/*
 * ==========
 * CUBE impl of a vector array system
 * ==========
 */
#ifndef CUBESTRING_H
#define CUBESTRING_H
#include "cube_types.h"
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <libintl.h>

#ifdef WIN32
#ifndef __GNUC__
#pragma warning(3 : 4189)       // local variable is initialized but not referenced
#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4355) // 'this' : used in base member initializer list
#pragma warning(disable : 4996) // 'strncpy' was declared deprecated
#endif
#define strcasecmp(a, b) _stricmp(a, b)
#define strncasecmp(a, b, n) _strnicmp(a, b, n)
#define PATHDIV '\\'
#else
#define __cdecl
#define _vsnprintf vsnprintf
#define PATHDIV '/'
#endif

// easy safe strings
#define MAXSTRLEN 260
typedef char string[MAXSTRLEN];

static inline void vformatstring(char *d, const char *fmt, va_list v, int len = MAXSTRLEN)
{
    _vsnprintf(d, len, fmt, v);
    d[len - 1] = 0;
}

static inline char *copystring(char *d, const char *s, size_t len = MAXSTRLEN)
{
    strncpy(d, s, len);
    d[len - 1] = 0;
    return d;
}

static inline char *concatstring(char *d, const char *s, size_t len = MAXSTRLEN)
{
    size_t used = strlen(d);
    return used < len ? copystring(d + used, s, len - used) : d;
}

extern char *concatformatstring(char *d, const char *s, ...);

struct stringformatter
{
    char *buf;
    stringformatter(char *buf) 
        : buf((char *)buf) 
    {
    }
    ~stringformatter() 
    {
    }
    
    void operator()(const char *fmt, ...)
    {
        va_list v;
        va_start(v, fmt);
        vformatstring(buf, fmt, v);
        va_end(v);
    }
};

#define formatstring(d) stringformatter((char *)d)
#define defformatstring(d) \
    string d;              \
    formatstring(d)
#define defvformatstring(d, last, fmt) \
    string d;                          \
    {                                  \
        va_list ap;                    \
        va_start(ap, last);            \
        vformatstring(d, fmt, ap);     \
        va_end(ap);                    \
    }

static inline char *strcaps(const char *s, bool on)
{
    static string r;
    char *o = r;
    if (on)
        while (*s && o < &r[sizeof(r) - 1])
            *o++ = toupper(*s++);
    else
        while (*s && o < &r[sizeof(r) - 1])
            *o++ = tolower(*s++);
    *o = '\0';
    return r;
}

static inline bool issimilar(char s, char d)
{
    s = tolower(s);
    d = tolower(d);
    if (s == d)
        return true;
    switch (d)
    {
    case 'a':
        if (s == '@' || s == '4')
            return true;
        break;
    case 'c':
        if (s == 'k')
            return true;
        break;
    case 'e':
        if (s == '3')
            return true;
        break;
    case 'i':
        if (s == '!' || s == '1')
            return true;
        break;
    case 'o':
        if (s == '0')
            return true;
        break;
    case 's':
        if (s == '$' || s == '5')
            return true;
        break;
    case 't':
        if (s == '7')
            return true;
        break;
    case 'u':
        if (s == '#')
            return true;
        break;
    }
    return false;
}

static inline bool findpattern(char *s, char *d) // returns true if there is more than 80% of similarity
{
    int len, hit = 0;
    if (!d || (len = strlen(d)) < 1)
        return false;
    char *dp = d, *s_end = s + strlen(s);
    while (s != s_end)
    {
        if (*s == ' ') // spaces separate words
        {
            if (!issimilar(*(s + 1), *dp))
            {
                dp = d;
                hit = 0;
            } // d e t e c t  i t
        }
        else if (issimilar(*s, *dp))
        {
            dp++;
            hit++;
        }                 // hit!
        else if (hit > 0) // this is not a pair, but there is a previous pattern
        {
            if (*s == '.' || *s == *(s - 1) || issimilar(*(s + 1), *dp))
                ; // separator or typo (do nothing)
            else if (issimilar(*(s + 1), *(dp + 1)) || *s == '*')
                dp++; // wild card or typo
            else
                hit--; // maybe this is nothing
        }
        else
            dp = d; // nothing here
        s++;        // walk on the string
        if (hit && 5 * hit > 4 * len)
            return true; // found it!
    }
    return false;
}

static inline char *newstring(size_t l)
{
    return new char[l + 1];
}

static inline char *newstring(const char *s, size_t l)
{
    return copystring(newstring(l), s, l + 1);
}

static inline char *newstring(const char *s)
{
    return newstring(s, strlen(s));
}

static inline char *newstringbuf()
{
    return newstring(MAXSTRLEN - 1);
}

static inline char *newstringbuf(const char *s)
{
    return newstring(s, MAXSTRLEN - 1);
}

#ifndef STANDALONE
static inline const char *_gettext(const char *msgid)
{
    if (msgid && msgid[0] != '\0') {
        return gettext(msgid);
    }
    return "";
}
#endif

#endif