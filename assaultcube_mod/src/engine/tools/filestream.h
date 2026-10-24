#ifndef CUBEFILESTREAM_H
#define CUBEFILESTREAM_H
#include "pch.h"
#include "cube_types.h"
#include "cube_vector.h"

/* workaround for some C platforms that have these two functions as macros - not used anywhere */
#ifdef getchar
#undef getchar
#endif
#ifdef putchar
#undef putchar
#endif

const int islittleendian = 1;
#ifdef SDL_BYTEORDER
#define endianswap16 SDL_Swap16
#define endianswap32 SDL_Swap32
#else
inline ushort endianswap16(ushort n) { return (n<<8) | (n>>8); }
inline uint endianswap32(uint n) { return (n<<24) | (n>>24) | ((n>>8)&0xFF00) | ((n<<8)&0xFF0000); }
#endif
template<class T> inline T endianswap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = endianswap32(conv.i); return conv.t; }
template<> inline ushort endianswap<ushort>(ushort n) { return endianswap16(n); }
template<> inline short endianswap<short>(short n) { return endianswap16(n); }
template<> inline uint endianswap<uint>(uint n) { return endianswap32(n); }
template<> inline int endianswap<int>(int n) { return endianswap32(n); }
template<class T> inline void endianswap(T *buf, int len) { for(T *end = &buf[len]; buf < end; buf++) *buf = endianswap(*buf); }
template<class T> inline T endiansame(T n) { return n; }
template<class T> inline void endiansame(T *buf, int len) {}
#ifdef SDL_BYTEORDER
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define lilswap endiansame
#define bigswap endianswap
#else
#define lilswap endianswap
#define bigswap endiansame
#endif
#else
template<class T> inline T lilswap(T n) { return *(const uchar *)&islittleendian ? n : endianswap(n); }
template<class T> inline void lilswap(T *buf, int len) { if(!*(const uchar *)&islittleendian) endianswap(buf, len); }
template<class T> inline T bigswap(T n) { return *(const uchar *)&islittleendian ? endianswap(n) : n; }
template<class T> inline void bigswap(T *buf, int len) { if(*(const uchar *)&islittleendian) endianswap(buf, len); }
#endif

struct stream
{
    stream() {}
    virtual ~stream() {}
    virtual void close() = 0;
    virtual bool end() = 0;
    virtual long tell() { return -1; }
    virtual bool seek(long offset, int whence = SEEK_SET) { return false; }
    virtual long size();
    virtual int read(void *buf, int len) { return 0; }
    virtual int write(const void *buf, int len) { return 0; }
    virtual int getchar()
    {
        uchar c;
        return read(&c, 1) == 1 ? c : -1;
    }
    virtual bool putchar(int n)
    {
        uchar c = n;
        return write(&c, 1) == 1;
    }
    virtual bool getline(char *str, int len);
    virtual bool putstring(const char *str)
    {
        int len = (int)strlen(str);
        return write(str, len) == len;
    }
    virtual bool putline(const char *str) { return putstring(str) && putchar('\n'); }
    virtual int printf(const char *fmt, ...) { return -1; }
    virtual uint getcrc() { return 0; }

    template <class T>
    bool put(T n) { return write(&n, sizeof(n)) == sizeof(n); }
    template <class T>
    bool putlil(T n) { return put<T>(lilswap(n)); }
    template <class T>
    bool putbig(T n) { return put<T>(bigswap(n)); }

    template <class T>
    T get()
    {
        T n;
        return read(&n, sizeof(n)) == sizeof(n) ? n : 0;
    }
    template <class T>
    T getlil() { return lilswap(get<T>()); }
    template <class T>
    T getbig() { return bigswap(get<T>()); }

#ifndef STANDALONE
    SDL_RWops *rwops();
#endif
};

char *path(char *s);
char *path(const char *s, bool copy);
char *unixpath(char *s);
const char *behindpath(const char *s);
const char *parentdir(const char *directory);
bool fileexists(const char *path, const char *mode);
bool createdir(const char *path);
size_t fixpackagedir(char *dir);
void sethomedir(const char *dir);
void addpackagedir(const char *dir);
const char *findfile(const char *filename, const char *mode);
int getfilesize(const char *filename);
stream *openrawfile(const char *filename, const char *mode);
stream *openfile(const char *filename, const char *mode);
stream *opentempfile(const char *filename, const char *mode);
stream *opengzfile(const char *filename, const char *mode, stream *file = NULL, int level = Z_BEST_COMPRESSION);
stream *openzipfile(const char *filename, const char *mode);
char *loadfile(const char *fn, int *size, const char *mode = NULL);
bool listdir(const char *dir, const char *ext, vector<char *> &files);
int listfiles(const char *dir, const char *ext, vector<char *> &files);
int listzipfiles(const char *dir, const char *ext, vector<char *> &files);
bool delfile(const char *path);
bool copyfile(const char *source, const char *destination);
bool preparedir(const char *destination);
bool addzip(const char *name, const char *mount = NULL, const char *strip = NULL, bool extract = false, int type = -1);
bool removezip(const char *name);

#endif