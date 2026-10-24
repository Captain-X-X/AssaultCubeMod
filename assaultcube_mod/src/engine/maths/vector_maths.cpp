#include "vector_maths.h"
#include "maths_func.h"
#include <math.h>
#include <cmath>

/*
    MMaths: vec2_t
*/
vec2_t::vec2_t(float _v)
    : x(_v), y(_v) {}

vec2_t::vec2_t(float _x, float _y)
    : x(_x), y(_y) {}

vec2_t vec2_t::operator*(float _v)
{
    x *= _v;
    y *= _v;
    return *this;
}

vec2_t vec2_t::operator/(float _v)
{
    x /= _v;
    y /= _v;
    return *this;
}

vec2_t vec2_t::operator*(const vec2_t &_v)
{
    x *= _v.x;
    y *= _v.y;
    return *this;
}

vec2_t vec2_t::operator/(const vec2_t &_v)
{
    x /= _v.x;
    y /= _v.y;
    return *this;
}

/*
    MMaths: vec3_t
*/
vec3_t::vec3_t()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}
vec3_t::vec3_t(float a, float b, float c)
    : x(a), y(b), z(c)
{
}

vec3_t::vec3_t(float *v)
    : x(v[0]), y(v[1]), z(v[2])
{
}

vec3_t::vec3_t(const vec2_t &_v2, float _z)
{
    this->x = _v2.x;
    this->y = _v2.y;
    this->z = _z;
}

float &vec3_t::operator[](int i)
{
    return v[i];
}

float vec3_t::operator[](int i) const
{
    return v[i];
}

bool vec3_t::iszero() const
{
    return (
        x == 0.0f &&
        y == 0.0f &&
        z == 0.0f);
}

bool vec3_t::operator==(const vec3_t &o) const
{
    return (
        x == o.x &&
        y == o.y &&
        z == o.z);
}

bool vec3_t::operator!=(const vec3_t &o) const
{
    return (
        x != o.x ||
        y != o.y ||
        z != o.z);
}

vec3_t vec3_t::operator-(const vec3_t &_v) const
{
    return vec3_t(
        this->x - _v.x,
        this->y - _v.y,
        this->z - _v.z);
}

vec3_t vec3_t::operator-() const
{
    return vec3_t(-x, -y, -z);
}

vec3_t &vec3_t::mul(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

vec3_t &vec3_t::div(float f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

vec3_t &vec3_t::add(float f)
{
    x += f;
    y += f;
    z += f;
    return *this;
}

vec3_t &vec3_t::sub(float f)
{
    x -= f;
    y -= f;
    z -= f;
    return *this;
}

vec3_t &vec3_t::add(const vec3_t &o)
{
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
}

vec3_t &vec3_t::sub(const vec3_t &o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

float vec3_t::squaredlen() const
{
    return (
        x * x +
        y * y +
        z * z);
}

float vec3_t::sqrxy() const
{
    return (x * x + y * y);
}

float vec3_t::dot(const vec3_t &o) const
{
    return (
        x * o.x +
        y * o.y +
        z * o.z);
}

float vec3_t::dotxy(const vec3_t &o) const
{
    return (x * o.x + y * o.y);
}

float vec3_t::magnitude() const
{
    return sqrtf(squaredlen());
}

vec3_t &vec3_t::normalize()
{
    div(magnitude());
    return *this;
}

// should NOT be used
float vec3_t::fmag() const
{
    return Maths::fSqrt(squaredlen());
}

float vec3_t::ufmag() const
{
    return Maths::ufSqrt(squaredlen());
}

float vec3_t::fmagxy() const
{
    return Maths::fSqrt(x * x + y * y);
}

float vec3_t::ufmagxy() const
{
    return Maths::ufSqrt(x * x + y * y);
}

float vec3_t::dist(const vec3_t &e) const
{
    vec3_t t;
    return dist(e, t);
}

float vec3_t::dist(const vec3_t &e, vec3_t &t) const
{
    t = *this;
    t.sub(e);
    return t.magnitude();
}

float vec3_t::distxy(const vec3_t &e) const
{
    float dx = e.x - x, dy = e.y - y;
    return sqrtf(dx * dx + dy * dy);
}

float vec3_t::magnitudexy() const
{
    return sqrtf(x * x + y * y);
}

bool vec3_t::reject(const vec3_t &o, float max) const
{
    return (
        x > o.x + max ||
        x < o.x - max ||
        y > o.y + max ||
        y < o.y - max);
}

vec3_t vec3_t::cross(const vec3_t &a, const vec3_t &b)
{
    vec3_t retVec = vec3_t(0.0f, 0.0f, 0.0f);
    // retVec.x = (a.y * b.z - a.z * b.y);
    // retVec.y = (a.z * b.x - a.x * b.z);
    // retVec.z = (a.x * b.y - a.y * b.x);
    retVec = (a - b);
    return retVec;
}

float vec3_t::cxy(const vec3_t &a)
{
    return x * a.y - y * a.x;
}

void vec3_t::rotate_around_z(float angle)
{
    *this = vec3_t(cosf(angle) * x - sinf(angle) * y, cosf(angle) * y + sinf(angle) * x, z);
}

void vec3_t::rotate_around_x(float angle)
{
    *this = vec3_t(x, cosf(angle) * y - sinf(angle) * z, cosf(angle) * z + sinf(angle) * y);
}

void vec3_t::rotate_around_y(float angle)
{
    *this = vec3_t(cosf(angle) * x - sinf(angle) * z, y, cosf(angle) * z + sinf(angle) * x);
}

vec3_t &vec3_t::rotate(float angle, const vec3_t &d)
{
    float c = cosf(angle), s = sinf(angle);
    return rotate(c, s, d);
}

vec3_t &vec3_t::rotate(float c, float s, const vec3_t &d)
{
    *this = vec3_t(x * (d.x * d.x * (1 - c) + c) + y * (d.x * d.y * (1 - c) - d.z * s) + z * (d.x * d.z * (1 - c) + d.y * s),
                   x * (d.y * d.x * (1 - c) + d.z * s) + y * (d.y * d.y * (1 - c) + c) + z * (d.y * d.z * (1 - c) - d.x * s),
                   x * (d.x * d.z * (1 - c) - d.y * s) + y * (d.y * d.z * (1 - c) + d.x * s) + z * (d.z * d.z * (1 - c) + c));
    return *this;
}

void vec3_t::orthogonal(const vec3_t &d)
{
    int i = fabs(d.x) > fabs(d.y) ? (fabs(d.x) > fabs(d.z) ? 0 : 2) : (fabs(d.y) > fabs(d.z) ? 1 : 2);
    v[i] = d[(i + 1) % 3];
    v[(i + 1) % 3] = -d[i];
    v[(i + 2) % 3] = 0;
}

bool htcmp(const vec3_t& x, const vec3_t& y)
{
    return x == y;
}

void vecfromyawpitch(float yaw, float pitch, int move, int strafe, vec3_t& m)
{
    if(move) {
        m.x = move*-sinf(RAD*yaw);
        m.y = move*cosf(RAD*yaw);
    }
    else {
        m.x = m.y = 0;
    }
    if(pitch) {
        m.x *= cosf(RAD*pitch);
        m.y *= cosf(RAD*pitch);
        m.z = move*sinf(RAD*pitch);
    }
    else {
        m.z = 0;
    }
    if(strafe) {
        m.x += strafe*cosf(RAD*yaw);
        m.y += strafe*sinf(RAD*yaw);
    }
}

void vectoyawpitch(const vec3_t& v, float &yaw, float &pitch)
{
    yaw = -atan2(v.x, v.y)/RAD;
    pitch = asin(v.z/v.magnitude())/RAD;
}

/*
    MMaths: vec4_t
*/
vec4_t::vec4_t(float _v)
    : vec4_t(_v, _v, _v, _v) {}

vec4_t::vec4_t(float _x, float _y, float _z, float _w)
    : x(_x), y(_y), z(_z), w(_w) {}

vec4_t::vec4_t(const vec3_t &_v, float _a)
    : vec4_t(_v.x, _v.y, _v.z, _a) {}

vec4_t vec4_t::operator*(float _v)
{
    x *= _v;
    y *= _v;
    z *= _v;
    w *= _v;
    return *this;
}
vec4_t vec4_t::operator/(float _v)
{
    x /= _v;
    y /= _v;
    z /= _v;
    w /= _v;
    return *this;
}
vec4_t vec4_t::operator*(const vec4_t &_v)
{
    x *= _v.x;
    y *= _v.y;
    z *= _v.z;
    w *= _v.w;
    return *this;
}

vec4_t vec4_t::operator/(const vec4_t &_v)
{
    x /= _v.x;
    y /= _v.y;
    z /= _v.z;
    w /= _v.w;
    return *this;
}

/*
 * ==========
 * vector of INT X,Y,Z
 * ==========
 */
ivec3_t::ivec3_t()
{
}

ivec3_t::ivec3_t(const vec3_t &v)
    : x(int(v.x)), y(int(v.y)), z(int(v.z))
{
}

ivec3_t::ivec3_t(int a, int b, int c)
    : x(a), y(b), z(c)
{
}

vec3_t ivec3_t::tovec() const
{
    return vec3_t(x, y, z);
}

int &ivec3_t::operator[](int i)
{
    return v[i];
}

int ivec3_t::operator[](int i) const
{
    return v[i];
}

bool ivec3_t::operator==(const ivec3_t &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

bool ivec3_t::operator!=(const ivec3_t &v) const
{
    return x != v.x || y != v.y || z != v.z;
}

ivec3_t &ivec3_t::mul(int n)
{
    x *= n;
    y *= n;
    z *= n;
    return *this;
}

ivec3_t &ivec3_t::div(int n)
{
    x /= n;
    y /= n;
    z /= n;
    return *this;
}

ivec3_t &ivec3_t::add(int n)
{
    x += n;
    y += n;
    z += n;
    return *this;
}

ivec3_t &ivec3_t::sub(int n)
{
    x -= n;
    y -= n;
    z -= n;
    return *this;
}

ivec3_t &ivec3_t::add(const ivec3_t &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

ivec3_t &ivec3_t::sub(const ivec3_t &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

ivec3_t &ivec3_t::mask(int n)
{
    x &= n;
    y &= n;
    z &= n;
    return *this;
}

ivec3_t &ivec3_t::cross(const ivec3_t &a, const ivec3_t &b)
{
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
    return *this;
}

int ivec3_t::dot(const ivec3_t &o) const
{
    return x * o.x + y * o.y + z * o.z;
}

bool htcmp(const ivec3_t &x, const ivec3_t &y)
{
    return x == y;
}

uint hthash(const ivec3_t &k)
{
    return k.x ^ k.y ^ k.z;
}

/*
 * ==========
 * vector of BYTE X,Y,Z
 * ==========
 */
bvec3_t::bvec3_t()
{
}

bvec3_t::bvec3_t(uchar x, uchar y, uchar z)
    : x(x), y(y), z(z)
{
}

bvec3_t::bvec3_t(const vec3_t &v)
    : x((uchar)((v.x + 1) * 255 / 2)), y((uchar)((v.y + 1) * 255 / 2)), z((uchar)((v.z + 1) * 255 / 2))
{
}

uchar &bvec3_t::operator[](int i)
{
    return v[i];
}

uchar bvec3_t::operator[](int i) const
{
    return v[i];
}

bool bvec3_t::operator==(const bvec3_t &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

bool bvec3_t::operator!=(const bvec3_t &v) const
{
    return x != v.x || y != v.y || z != v.z;
}

bool bvec3_t::iszero() const
{
    return x == 0 && y == 0 && z == 0;
}

vec3_t bvec3_t::tovec() const
{
    return vec3_t(x * (2.0f / 255.0f) - 1.0f, y * (2.0f / 255.0f) - 1.0f, z * (2.0f / 255.0f) - 1.0f);
}