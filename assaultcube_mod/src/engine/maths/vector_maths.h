#ifndef CMEVECTORMATHS_H
#define CMEVECTORMATHS_H
#include "maths_func.h"
#include "tools/cube_types.h"

/*
* ==========
* vector of X,Y
* ==========
*/
typedef struct vec2_t {
    float x;
    float y;

    vec2_t(float _v = 0.0f);
    vec2_t(float _x, float _y);

    vec2_t operator*(float _v);
    vec2_t operator/(float _v);
    vec2_t operator*(const vec2_t& _v);
    vec2_t operator/(const vec2_t& _v);
} Vector2, vec2;

/*
* ==========
* vector of X,Y,Z
* ==========
*/
typedef struct vec3_t {
    union {
        struct { float x, y, z; };
        float v[3];
        int i[3];
    };

    vec3_t();
    vec3_t(float a, float b, float c);
    vec3_t(float *v);
    vec3_t(const vec2_t& _v2, float _z);

    float &operator[](int i);
    float operator[](int i) const;

    bool iszero() const;
    bool operator==(const vec3_t &o) const;
    bool operator!=(const vec3_t &o) const;
    vec3_t operator-(const vec3_t& _v) const;
    vec3_t operator-() const;
    vec3_t &mul(float f);
    vec3_t &div(float f);
    vec3_t &add(float f);
    vec3_t &sub(float f);
    vec3_t &add(const vec3_t &o);
    vec3_t &sub(const vec3_t &o);
    float squaredlen() const;
    float sqrxy() const;
    float dot(const vec3_t &o) const;
    float dotxy(const vec3_t &o) const;
    float magnitude() const;
    vec3_t &normalize();

    // should NOT be used 
    float fmag() const;
    float ufmag() const;
    float fmagxy() const;
    float ufmagxy() const;
    float dist(const vec3_t &e) const;
    float dist(const vec3_t &e, vec3_t &t) const;
    float distxy(const vec3_t &e) const;
    float magnitudexy() const;
    bool reject(const vec3_t &o, float max) const;
    static vec3_t cross(const vec3_t &a, const vec3_t &b);
    float cxy(const vec3_t &a);
    void rotate_around_z(float angle);
    void rotate_around_x(float angle);
    void rotate_around_y(float angle);
    vec3_t &rotate(float angle, const vec3_t &d);
    vec3_t &rotate(float c, float s, const vec3_t &d);
    void orthogonal(const vec3_t &d);
} Vector3, vec, vec3;

bool htcmp(const vec3_t& x, const vec3_t& y);
void vecfromyawpitch(float yaw, float pitch, int move, int strafe, vec3_t& m);
void vectoyawpitch(const vec3_t& v, float &yaw, float &pitch);

/*
* ==========
* vector of X,Y,Z,W
* ==========
*/
typedef struct vec4_t {
    float x, y, z, w;

    vec4_t(float _v = 0.0f);
    vec4_t(float _x, float _y, float _z, float _w);
    vec4_t(const vec3_t& _v, float _a);
    vec4_t(float* mValue) : x(mValue[0]), y(mValue[1]), z(mValue[2]), w(mValue[3]) { }

    vec4_t operator*(float _v);
    vec4_t operator/(float _v);
    vec4_t operator*(const vec4_t& _v);
    vec4_t operator/(const vec4_t& _v);
} Vector4, vec4;

/*
* ==========
* vector of INT X,Y,Z
* ==========
*/
typedef struct ivec3_t {
    union {
        struct { int x, y, z; };
        int v[3];
    };

    ivec3_t();
    ivec3_t(const vec3_t &v);
    ivec3_t(int a, int b, int c);

    vec3_t tovec() const ;

    int &operator[](int i);
    int  operator[](int i) const;

    bool operator==(const ivec3_t &v) const;
    bool operator!=(const ivec3_t &v) const;
    ivec3_t &mul(int n);
    ivec3_t &div(int n);
    ivec3_t &add(int n);
    ivec3_t &sub(int n);
    ivec3_t &add(const ivec3_t &v);
    ivec3_t &sub(const ivec3_t &v);
    ivec3_t &mask(int n);
    ivec3_t &cross(const ivec3_t &a, const ivec3_t &b);
    int dot(const ivec3_t &o) const;
} ivec;

bool htcmp(const ivec3_t &x, const ivec3_t &y);
uint hthash(const ivec3_t &k);

/*
* ==========
* vector of BYTE X,Y,Z
* ==========
*/
typedef struct bvec3_t {
    union {
        struct { uchar x, y, z; };
        uchar v[3];
    };

    bvec3_t();
    bvec3_t(uchar x, uchar y, uchar z);
    bvec3_t(const vec3_t &v);

    uchar &operator[](int i);
    uchar  operator[](int i) const;
    bool operator==(const bvec3_t &v) const;
    bool operator!=(const bvec3_t &v) const;
    bool iszero() const;
    vec3_t tovec() const;
} bvec;

#endif