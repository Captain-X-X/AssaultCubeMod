/*
* ==========
* CUBE impl of a vector array system
* ==========
*/
#ifndef CUBEHASHTABLE_H
#define CUBEHASHTABLE_H
#include "cube_types.h"
#include <string.h>

//
static inline uint hthash(int key)
{
    return key;
}
static inline uint hthash(const char *key)
{
    uint h = 5381;
    for(int i = 0, k; (k = key[i]); i++) h = ((h<<5)+h)^k;    // bernstein k=33 xor
    return h;
}
static inline bool htcmp(const char *x, const char *y)
{
    return !strcmp(x, y);
}

static inline bool htcmp(int x, int y)
{
    return x==y;
}

static inline uint hthash(uint key)
{
    return key;
}

static inline bool htcmp(uint x, uint y)
{
    return x==y;
}

//
template <class K, class T>
struct hashtable
{
    typedef K key;
    typedef const K const_key;
    typedef T value;
    typedef const T const_value;

    enum {
        CHUNKSIZE = 16
    };

    struct chain {
        T data;
        K key;
        chain *next;
    };
    struct chainchunk {
        chain chains[CHUNKSIZE];
        chainchunk *next;
    };

    int size;
    int numelems;
    chain **table;
    chain *enumc;

    chainchunk *chunks;
    chain *unused;

    hashtable(int size = 1 << 10)
        : size(size)
    {
        numelems = 0;
        chunks = NULL;
        unused = NULL;
        table = new chain *[size];
        for (int i = 0; i < size; i++) {
            table[i] = NULL;
        }
    }

    ~hashtable() {
        if (table) {
            delete[] table;
            table = 0;
        }
        deletechunks();
    }

    void insertchains(chainchunk *emptychunk)
    {
        for (int i = 0; i < CHUNKSIZE - 1; i++) emptychunk->chains[i].next = &emptychunk->chains[i + 1];
        emptychunk->chains[CHUNKSIZE - 1].next = unused;
        unused = emptychunk->chains;
    }

    chain *insert(const K &key, uint h)
    {
        if (!unused)
        {
            chainchunk *chunk = new chainchunk;
            chunk->next = chunks;
            chunks = chunk;
            insertchains(chunk);
        }
        chain *c = unused;
        unused = unused->next;
        c->key = key;
        c->next = table[h];
        table[h] = c;
        numelems++;
        return c;
    }

#define HTFIND(success, fail)                 \
    uint h = hthash(key) & (size - 1);        \
    for (chain *c = table[h]; c; c = c->next) \
    {                                         \
        if (htcmp(key, c->key))               \
            return (success);                 \
    }                                         \
    return (fail);

    T *access(const K &key)
    {
        HTFIND(&c->data, NULL);
    }

    T &access(const K &key, const T &data)
    {
        HTFIND(c->data, insert(key, h)->data = data);
    }

    T &operator[](const K &key)
    {
        HTFIND(c->data, insert(key, h)->data);
    }

#undef HTFIND

    bool remove(const K &key)
    {
        uint h = hthash(key) & (size - 1);
        for (chain **p = &table[h], *c = table[h]; c; p = &c->next, c = c->next)
        {
            if (htcmp(key, c->key))
            {
                *p = c->next;
                c->data.~T();
                c->key.~K();
                new (&c->data) T;
                new (&c->key) K;
                c->next = unused;
                unused = c;
                numelems--;
                return true;
            }
        }
        return false;
    }

    void deletechunks()
    {
        for (chainchunk *nextchunk; chunks; chunks = nextchunk)
        {
            nextchunk = chunks->next;
            delete chunks;
        }
    }

    void clear(bool del = true)
    {
        for (int i = 0; i < size; i++) {
            table[i] = NULL;
        } 
        numelems = 0;
        unused = NULL;
        if (del) {
            deletechunks();
        }
        else {
            for (chainchunk *chunk = chunks; chunk; chunk = chunk->next) {
                insertchains(chunk);
            }
        }
    }
};

#endif