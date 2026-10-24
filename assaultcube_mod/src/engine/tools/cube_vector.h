/*
 * ==========
 * CUBE impl of a vector array system
 * ==========
 */
#ifndef CUBEVECTOR_H
#define CUBEVECTOR_H
#include "cube_types.h"
#include "maths/maths_func.h"
#include <memory.h>
#include <stdlib.h>

#define ASSERT_CVECTOR(c) if(c) {}

#if !defined(WIN32)
#define __cdecl
#endif

//
template <class T>
struct databuf
{
    enum
    {
        OVERREAD = 1 << 0,
        OVERWROTE = 1 << 1
    };

    T *buf;
    int len, maxlen;
    uchar flags;

    databuf() : buf(NULL), len(0), maxlen(0), flags(0) {}

    template <class U>
    databuf(T *buf, U maxlen) : buf(buf), len(0), maxlen((int)maxlen), flags(0) {}

    const T &get()
    {
        static T overreadval = 0;
        if (len < maxlen)
            return buf[len++];
        flags |= OVERREAD;
        return overreadval;
    }

    databuf subbuf(int sz)
    {
        sz = Maths::min(sz, maxlen - len);
        len += sz;
        return databuf(&buf[len - sz], sz);
    }

    void put(const T &val)
    {
        if (len < maxlen)
            buf[len++] = val;
        else
            flags |= OVERWROTE;
    }

    void put(const T *vals, int numvals)
    {
        if (maxlen - len < numvals)
            flags |= OVERWROTE;
        memcpy(&buf[len], vals, Maths::min(maxlen - len, numvals) * sizeof(T));
        len += Maths::min(maxlen - len, numvals);
    }

    int get(T *vals, int numvals)
    {
        int read = Maths::min(maxlen - len, numvals);
        if (read < numvals)
            flags |= OVERREAD;
        memcpy(vals, &buf[len], read * sizeof(T));
        len += read;
        return read;
    }

    int length() const { return len; }
    int remaining() const { return maxlen - len; }
    bool overread() const { return (flags & OVERREAD) != 0; }
    bool overwrote() const { return (flags & OVERWROTE) != 0; }

    void forceoverread()
    {
        len = maxlen;
        flags |= OVERREAD;
    }
};

typedef databuf<char> charbuf;
typedef databuf<uchar> ucharbuf;

template <class T>
struct vector
{
    static const int MINSIZE = 8;

    T *buf;
    int alen, ulen;

    vector() : buf(NULL), alen(0), ulen(0) {}
    vector(const vector &v) : buf(NULL), alen(0), ulen(0) { *this = v; }
    ~vector()
    {
        shrink(0);
        if (buf)
            delete[] (uchar *)buf;
    }

    vector<T> &operator=(const vector<T> &v)
    {
        shrink(0);
        if (v.length() > alen)
            growbuf(v.length());
        for (int i = 0; i < v.length(); i++) {
            add(v[i]);
        }
        return *this;
    }

    T &add(const T &x)
    {
        if (ulen == alen)
            growbuf(ulen + 1);
        new (&buf[ulen]) T(x);
        return buf[ulen++];
    }

    T &add()
    {
        if (ulen == alen)
            growbuf(ulen + 1);
        new (&buf[ulen]) T;
        return buf[ulen++];
    }

    T &dup()
    {
        if (ulen == alen)
            growbuf(ulen + 1);
        new (&buf[ulen]) T(buf[ulen - 1]);
        return buf[ulen++];
    }

    bool inrange(size_t i) const { return i < size_t(ulen); }
    bool inrange(int i) const { return i >= 0 && i < ulen; }

    T &pop() { return buf[--ulen]; }
    T &last() { return buf[ulen - 1]; }
    void drop() { buf[--ulen].~T(); }
    bool empty() const { return ulen == 0; }

    int capacity() const { return alen; }
    int length() const { return ulen; }
    T &operator[](int i)
    {
        ASSERT_CVECTOR(i >= 0 && i < ulen);
        return buf[i];
    }
    const T &operator[](int i) const
    {
        ASSERT_CVECTOR(i >= 0 && i < ulen);
        return buf[i];
    }

    void shrink(int i)
    {
        ASSERT_CVECTOR(i <= ulen);
        while (ulen > i)
            drop();
    }
    void setsize(int i)
    {
        ASSERT_CVECTOR(i <= ulen);
        ulen = i;
    }

    void deletecontents()
    {
        while (!empty())
            delete pop();
    }
    void deletearrays()
    {
        while (!empty())
            delete[] pop();
    }

    T *getbuf() { return buf; }
    const T *getbuf() const { return buf; }
    bool inbuf(const T *e) const { return e >= buf && e < &buf[ulen]; }

    template <class ST>
    void sort(int(__cdecl *cf)(ST *, ST *), int i = 0, int n = -1)
    {
        qsort(&buf[i], n < 0 ? ulen : n, sizeof(T), (int(__cdecl *)(const void *, const void *))cf);
    }

    template <class ST>
    T *search(T *key, int(__cdecl *cf)(ST *, ST *), int i = 0, int n = -1)
    {
        return (T *)bsearch(key, &buf[i], n < 0 ? ulen : n, sizeof(T), (int(__cdecl *)(const void *, const void *))cf);
    }

    void growbuf(int sz)
    {
        int olen = alen;
        if (!alen)
            alen = Maths::max(MINSIZE, sz);
        else
            while (alen < sz)
                alen *= 2;
        if (alen <= olen)
            return;
        uchar *newbuf = new uchar[alen * sizeof(T)];
        if (olen > 0)
        {
            memcpy(newbuf, buf, olen * sizeof(T));
            delete[] (uchar *)buf;
        }
        buf = (T *)newbuf;
    }

    databuf<T> reserve(int sz)
    {
        if (ulen + sz > alen)
            growbuf(ulen + sz);
        return databuf<T>(&buf[ulen], sz);
    }

    void advance(int sz)
    {
        ulen += sz;
    }

    void addbuf(const databuf<T> &p)
    {
        advance(p.length());
    }

    T *pad(int n)
    {
        T *buf = reserve(n).buf;
        advance(n);
        return buf;
    }

    void put(const T &v) { add(v); }

    void put(const T *v, int n)
    {
        databuf<T> buf = reserve(n);
        buf.put(v, n);
        addbuf(buf);
    }

    void remove(int i, int n)
    {
        for (int p = i + n; p < ulen; p++)
            buf[p - n] = buf[p];
        ulen -= n;
    }

    T remove(int i)
    {
        T e = buf[i];
        for (int p = i + 1; p < ulen; p++)
            buf[p - 1] = buf[p];
        ulen--;
        return e;
    }

    int find(const T &o)
    {
        for (int i = 0; i < ulen; i++) {
            if (buf[i] == o) { return i; }
        }
        return -1;
    }

    void removeobj(const T &o)
    {
        for (int i = 0; i < ulen; i++) if (buf[i] == o) remove(i--);
    }

    void replacewithlast(const T &o)
    {
        if (!ulen)
            return;
        for (int i = 0; i < ulen - 1; i++) if (buf[i] == o)
        {
            buf[i] = buf[ulen - 1];
        }
        ulen--;
    }

    T &insert(int i, const T &e)
    {
        add(T());
        for (int p = ulen - 1; p > i; p--)
            buf[p] = buf[p - 1];
        buf[i] = e;
        return buf[i];
    }

    T *insert(int i, const T *e, int n)
    {
        if (ulen + n > alen)
            growbuf(ulen + n);
        for (int j = 0; j < n; j++) add(T());
        for (int p = ulen - 1; p >= i + n; p--)
            buf[p] = buf[p - n];
        for (int j = 0; j < n; j++) buf[i + j] = e[j];
        return &buf[i];
    }
};

#endif