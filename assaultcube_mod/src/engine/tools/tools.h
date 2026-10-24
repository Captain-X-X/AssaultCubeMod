// generic useful stuff for any C++ program

#ifndef _TOOLS_H
#define _TOOLS_H
#include "cube_vector.h"
#include "cube_hashtable.h"
#include "cube_string.h"
#include "cube_types.h"
#include "lib/enet/include/enet/enet.h"

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#ifdef _DEBUG
#ifdef __GNUC__
#define ASSERT(c)      \
    if (!(c))          \
    {                  \
        asm("int $3"); \
    }
#else
#define ASSERT(c)   \
    if (!(c))       \
    {               \
        __asm int 3 \
    }
#endif

#include <iostream>
#define DEBUG(v)                                                                                                  \
    if (DEBUGCOND)                                                                                                \
    {                                                                                                             \
        std::cout << behindpath(__FILE__) << ":" << __LINE__ << " " << __FUNCTION__ << "(..) " << v << std::endl; \
    }
#define DEBUGVAR(v)                                                                                                              \
    if (DEBUGCOND)                                                                                                               \
    {                                                                                                                            \
        std::cout << behindpath(__FILE__) << ":" << __LINE__ << " " << __FUNCTION__ << "(..) " << #v << " = " << v << std::endl; \
    }
#else
#define DEBUG(v) \
    {            \
    }
#define DEBUGVAR(v) \
    {               \
    }
#define ASSERT(c) \
    if (c)        \
    {             \
    }
#endif

#ifdef swap
#undef swap
#endif
template <class T>
static inline void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}
#define rnd(x) ((int)(randomMT() & 0xFFFFFF) % (x))
#define rndscale(x) (float((randomMT() & 0xFFFFFF) * double(x) / double(0xFFFFFF)))
#define detrnd(s, x) ((int)(((((uint)(s)) * 1103515245 + 12345) >> 16) % (x)))

#define loop(v, m) for (int v = 0; v < int(m); v++)
#define loopi(m) loop(i, m)
#define loopj(m) loop(j, m)
#define loopk(m) loop(k, m)
#define loopl(m) loop(l, m)

#define DELETEP(p) \
    if (p)         \
    {              \
        delete p;  \
        p = 0;     \
    }
#define DELETEA(p)  \
    if (p)          \
    {               \
        delete[] p; \
        p = 0;      \
    }

#ifdef WIN32
#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265

#ifndef __GNUC__
#pragma warning(3 : 4189)       // local variable is initialized but not referenced
#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4355) // 'this' : used in base member initializer list
#pragma warning(disable : 4996) // 'strncpy' was declared deprecated
#endif
#endif

#define MAXMAPNAMELEN 64
inline bool validmapname(char *s)
{
    if (strlen(s) > MAXMAPNAMELEN)
        return false;
    while (*s != '\0')
    {
        if (!isalnum(*s) && *s != '_' && *s != '-' && *s != '.')
            return false;
        ++s;
    }
    return true;
}

#define loopv(v) for (int i = 0; i < (v).length(); i++)
#define loopvj(v) for (int j = 0; j < (v).length(); j++)
#define loopvk(v) for (int k = 0; k < (v).length(); k++)
#define loopvrev(v) for (int i = (v).length() - 1; i >= 0; i--)
#define loopvjrev(v) for (int j = (v).length() - 1; i >= 0; i--)

struct packetbuf : ucharbuf
{
    ENetPacket *packet;
    int growth;

    packetbuf(ENetPacket *packet) : ucharbuf(packet->data, packet->dataLength), packet(packet), growth(0) {}
    packetbuf(int growth, int pflags = 0) : growth(growth)
    {
        packet = enet_packet_create(NULL, growth, pflags);
        buf = (uchar *)packet->data;
        maxlen = packet->dataLength;
    }
    ~packetbuf() { cleanup(); }

    void reliable() { packet->flags |= ENET_PACKET_FLAG_RELIABLE; }

    void resize(int n)
    {
        enet_packet_resize(packet, n);
        buf = (uchar *)packet->data;
        maxlen = packet->dataLength;
    }

    void checkspace(int n)
    {
        if (len + n > maxlen && packet && growth > 0)
            resize(Maths::max(len + n, maxlen + growth));
    }

    ucharbuf subbuf(int sz)
    {
        checkspace(sz);
        return ucharbuf::subbuf(sz);
    }

    void put(const uchar &val)
    {
        checkspace(1);
        ucharbuf::put(val);
    }

    void put(const uchar *vals, int numvals)
    {
        checkspace(numvals);
        ucharbuf::put(vals, numvals);
    }

    bool overwrote() const { return false; }

    ENetPacket *finalize()
    {
        resize(len);
        return packet;
    }

    void cleanup()
    {
        if (growth > 0 && packet && !packet->referenceCount)
        {
            enet_packet_destroy(packet);
            packet = NULL;
            buf = NULL;
            len = maxlen = 0;
        }
    }
};

template <class T>
struct bitbuf
{
    T &q;
    int blen;

    bitbuf(T &oq) : q(oq), blen(0) {}

    int getbits(int n)
    {
        int res = 0, p = 0;
        while (n > 0)
        {
            if (!blen)
                q.get();
            int r = 8 - blen;
            if (r > n)
                r = n;
            res |= ((q.buf[q.len - 1] >> blen) & ((1 << r) - 1)) << p;
            n -= r;
            p += r;
            blen = (blen + r) % 8;
        }
        return res;
    }

    void putbits(int n, int v)
    {
        while (n > 0)
        {
            if (!blen)
            {
                q.put(0);
                if (q.overwrote())
                    return;
            }
            int r = 8 - blen;
            if (r > n)
                r = n;
            q.buf[q.len - 1] |= (v & ((1 << r) - 1)) << blen;
            n -= r;
            v >>= r;
            blen = (blen + r) % 8;
        }
    }

    int rembits() { return (8 - blen) % 8; } // 0..7 bits remaining in current byte
};

template <class T, int SIZE>
struct ringbuf
{
    int index, len;
    T data[SIZE];

    ringbuf() { clear(); }

    void clear()
    {
        index = len = 0;
    }

    bool empty() const { return !len; }

    int maxsize() const { return SIZE; }
    int length() const { return len; }

    T &remove()
    {
        int start = index - len;
        if (start < 0)
            start += SIZE;
        len--;
        return data[start];
    }

    T &add(const T &e)
    {
        T &t = data[index];
        t = e;
        index++;
        if (index >= SIZE)
            index = 0;
        if (len < SIZE)
            len++;
        return t;
    }

    T &add() { return add(T()); }

    T &operator[](int i)
    {
        int start = index - len;
        if (start < 0)
            start += SIZE;
        i += start;
        if (i >= SIZE)
            i -= SIZE;
        return data[i];
    }

    const T &operator[](int i) const
    {
        int start = index - len;
        if (start < 0)
            start += SIZE;
        i += start;
        if (i >= SIZE)
            i -= SIZE;
        return data[i];
    }

    int find(const T &o)
    {
        loopi(len) if (data[i] == o) return i;
        return -1;
    }
};

#define itoa(s, i) sprintf(s, "%d", i)
#define ftoa(s, f) sprintf(s, (f) == int(f) ? "%.1f" : "%.7g", f)

#define enumeratekt(ht, k, e, t, f, b)                                           \
    loopi((ht).size) for (hashtable<k, t>::chain *enumc = (ht).table[i]; enumc;) \
    {                                                                            \
        hashtable<k, t>::const_key &e = enumc->key;                              \
        t &f = enumc->data;                                                      \
        enumc = enumc->next;                                                     \
        b;                                                                       \
    }
#define enumeratek(ht, k, e, b)                                    \
    loopi((ht).size) for ((ht).enumc = (ht).table[i]; (ht).enumc;) \
    {                                                              \
        k &e = (ht).enumc->key;                                    \
        (ht).enumc = (ht).enumc->next;                             \
        b;                                                         \
    }
#define enumerate(ht, t, e, b)                                     \
    loopi((ht).size) for ((ht).enumc = (ht).table[i]; (ht).enumc;) \
    {                                                              \
        t &e = (ht).enumc->data;                                   \
        (ht).enumc = (ht).enumc->next;                             \
        b;                                                         \
    }

#ifndef STANDALONE
// ease time measurement
struct stopwatch
{
    int millis;

    stopwatch() : millis(-1) {}

    void start()
    {
        millis = SDL_GetTicks();
    }

    // returns elapsed time
    int stop()
    {
        ASSERT(millis >= 0);
        int time = SDL_GetTicks() - millis;
        millis = -1;
        return time;
    }
};
#endif

#define _(s) _gettext(s)

#define uint2ip(address, ip)                   \
    uchar ip[4];                               \
    if (isbigendian())                         \
    {                                          \
        enet_uint32 big = endianswap(address); \
        memcpy(&ip, &big, 4);                  \
    }                                          \
    else                                       \
        memcpy(&ip, &address, 4);

extern const char *timestring(bool local = false, const char *fmt = NULL);
extern const char *asctime();
extern const char *numtime();

extern struct mapstats *loadmapstats(const char *filename, bool getlayout);
extern bool cmpb(void *b, int n, enet_uint32 c);
extern bool cmpf(char *fn, enet_uint32 c);
extern enet_uint32 adler(unsigned char *data, size_t len);
extern void endianswap(void *, int, int);
extern bool isbigendian();
extern void strtoupper(char *t, const char *s = NULL);
extern void seedMT(uint seed);
extern uint randomMT(void);

struct iprange
{
    enet_uint32 lr, ur;
};
extern const char *atoip(const char *s, enet_uint32 *ip);
extern const char *atoipr(const char *s, iprange *ir);
extern const char *iptoa(const enet_uint32 ip);
extern const char *iprtoa(const struct iprange &ipr);
extern int cmpiprange(const struct iprange *a, const struct iprange *b);
extern int cmpipmatch(const struct iprange *a, const struct iprange *b);
extern const char *hiddenpwd(const char *pwd, int showchars = 0);

#if defined(WIN32) && !defined(_DEBUG) && !defined(__GNUC__)
extern void stackdumper(unsigned int type, EXCEPTION_POINTERS *ep);
#endif

#endif
