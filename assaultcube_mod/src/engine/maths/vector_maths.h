#ifndef VECTORMATHS_H
#define VECTORMATHS_H
#include "maths_func.h"
#include "tools/cube_types.h"

/*
* ==========
* Vector FLOAT 2
* ==========
*/
class Vector2 
{
public:
    float x, y;

public:
    // Vector2() { }
    Vector2(float _x = 0.0f, float _y = 0.0f);
    Vector2(float _v);

    Vector2 operator*(float _v);
    Vector2 operator/(float _v);
    Vector2 operator*(const Vector2& _v);
    Vector2 operator/(const Vector2& _v);
};
typedef Vector2 vec2;
typedef Vector2 vec2_t;

/*
* ==========
* Vector FLOAT 3
* ==========
*/
class Vector3 
{
public:
    union {
        struct { float x, y, z; };
        float v[3];
        int i[3];
    };

public:
    // Vector3() { }
    Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);
    Vector3(float *v);
    Vector3(const vec2_t& _v2, float _z);

    float &operator[](int i);
    float operator[](int i) const;

    bool iszero() const;
    bool operator==(const Vector3 &o) const;
    bool operator!=(const Vector3 &o) const;
    Vector3 operator-(const Vector3& _v) const;
    Vector3 operator-() const;
    Vector3 &mul(float f);
    Vector3 &div(float f);
    Vector3 &add(float f);
    Vector3 &sub(float f);
    Vector3 &add(const Vector3 &o);
    Vector3 &sub(const Vector3 &o);
    float squaredlen() const;
    float sqrxy() const;
    float dot(const Vector3 &o) const;
    float dotxy(const Vector3 &o) const;
    float magnitude() const;
    Vector3 &normalize();

    // should NOT be used 
    float fmag() const;
    float ufmag() const;
    float fmagxy() const;
    float ufmagxy() const;
    float dist(const Vector3 &e) const;
    float dist(const Vector3 &e, Vector3 &t) const;
    float distxy(const Vector3 &e) const;
    float magnitudexy() const;
    bool reject(const Vector3 &o, float max) const;
    static Vector3 cross(const Vector3 &a, const Vector3 &b);
    float cxy(const Vector3 &a);
    void rotate_around_z(float angle);
    void rotate_around_x(float angle);
    void rotate_around_y(float angle);
    Vector3 &rotate(float angle, const Vector3 &d);
    Vector3 &rotate(float c, float s, const Vector3 &d);
    void orthogonal(const Vector3 &d);
};
typedef Vector3 vec;
typedef Vector3 vec3;
typedef Vector3 vec3_t;

bool htcmp(const Vector3& x, const Vector3& y);
void vecfromyawpitch(float yaw, float pitch, int move, int strafe, Vector3& m);
void vectoyawpitch(const Vector3& v, float &yaw, float &pitch);

/*
* ==========
* Vector FLOAT 4
* ==========
*/
class Vector4 
{
public:
    float x, y, z, w;

public:
    // Vector4() { }
    Vector4(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f);
    // Vector4(float _v = 0.0f);
    Vector4(float* _v);
    Vector4(const Vector3& _v, float _a);

    Vector4 operator*(float _v);
    Vector4 operator/(float _v);
    Vector4 operator*(const Vector4& _v);
    Vector4 operator/(const Vector4& _v);
};
typedef Vector4 vec4;
typedef Vector4 vec4_t;

/*
* ==========
* vector of INT X,Y,Z
* ==========
*/
class ivec3 
{
public:
    union {
        struct { int x, y, z; };
        int v[3];
    };

public:
    ivec3(int _x = 0, int _y = 0, int _z = 0);
    ivec3(const Vector3 &v);
    ~ivec3();

    Vector3 tovec() const ;

    int &operator[](int i);
    int  operator[](int i) const;

    bool operator==(const ivec3 &v) const;
    bool operator!=(const ivec3 &v) const;
    ivec3 &mul(int n);
    ivec3 &div(int n);
    ivec3 &add(int n);
    ivec3 &sub(int n);
    ivec3 &add(const ivec3 &v);
    ivec3 &sub(const ivec3 &v);
    ivec3 &mask(int n);
    ivec3 &cross(const ivec3 &a, const ivec3 &b);
    int dot(const ivec3 &o) const;
};
typedef ivec3 ivec;
typedef ivec3 ivec3_t;

bool htcmp(const ivec3 &x, const ivec3 &y);
uint hthash(const ivec3 &k);

/*
* ==========
* vector of BYTE X,Y,Z
* ==========
*/
class bvec3 
{
public:
    union {
        struct { uchar x, y, z; };
        uchar v[3];
    };

public:
    bvec3(uchar x = 0, uchar y = 0, uchar z = 0);
    bvec3(const Vector3 &v);
    ~bvec3();

    uchar &operator[](int i);
    uchar  operator[](int i) const;
    bool operator==(const bvec3 &v) const;
    bool operator!=(const bvec3 &v) const;
    bool iszero() const;
    Vector3 tovec() const;
};
typedef bvec3 bvec;
typedef bvec3 bvec3_t;

#endif