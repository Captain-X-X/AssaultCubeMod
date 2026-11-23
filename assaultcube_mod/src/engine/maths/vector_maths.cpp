#include "vector_maths.h"
#include "maths_func.h"
#include <math.h>
#include <cmath>

/*
    MMaths: vec2_t
*/
Vector2::Vector2(float _x, float _y)
    : x(_x), y(_y) 
{

}

Vector2::Vector2(float _v)
    : x(_v), y(_v) 
{

}

vec2_t Vector2::operator*(float _v)
{
    x *= _v;
    y *= _v;
    return *this;
}

vec2_t Vector2::operator/(float _v)
{
    x /= _v;
    y /= _v;
    return *this;
}

vec2_t Vector2::operator*(const vec2_t &_v)
{
    x *= _v.x;
    y *= _v.y;
    return *this;
}

vec2_t Vector2::operator/(const vec2_t &_v)
{
    x /= _v.x;
    y /= _v.y;
    return *this;
}

/*
    MMaths: vec3_t
*/
Vector3::Vector3(float a, float b, float c)
    : x(a), y(b), z(c)
{

}

Vector3::Vector3(float *v)
    : x(v[0]), y(v[1]), z(v[2])
{
}

Vector3::Vector3(const Vector2 &_v2, float _z)
{
    this->x = _v2.x;
    this->y = _v2.y;
    this->z = _z;
}

float &Vector3::operator[](int i)
{
    return v[i];
}

float Vector3::operator[](int i) const
{
    return v[i];
}

bool Vector3::iszero() const
{
    return (
        x == 0.0f &&
        y == 0.0f &&
        z == 0.0f
);
}

bool Vector3::operator==(const vec3_t &o) const
{
    return (
        x == o.x &&
        y == o.y &&
        z == o.z
    );
}

bool Vector3::operator!=(const vec3_t &o) const
{
    return (
        x != o.x ||
        y != o.y ||
        z != o.z
    );
}

vec3_t Vector3::operator-(const vec3_t &_v) const
{
    return vec3_t(
        this->x - _v.x,
        this->y - _v.y,
        this->z - _v.z
    );
}

vec3_t Vector3::operator-() const
{
    return vec3_t(-x, -y, -z);
}

vec3_t &Vector3::mul(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

vec3_t &Vector3::div(float f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

vec3_t &Vector3::add(float f)
{
    x += f;
    y += f;
    z += f;
    return *this;
}

vec3_t &Vector3::sub(float f)
{
    x -= f;
    y -= f;
    z -= f;
    return *this;
}

vec3_t &Vector3::add(const vec3_t &o)
{
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
}

vec3_t &Vector3::sub(const vec3_t &o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

float Vector3::squaredlen() const
{
    return (
        x * x +
        y * y +
        z * z
    );
}

float Vector3::sqrxy() const
{
    return (x * x + y * y);
}

float Vector3::dot(const vec3_t &o) const
{
    return (
        x * o.x +
        y * o.y +
        z * o.z
    );
}

float Vector3::dotxy(const vec3_t &o) const
{
    return (x * o.x + y * o.y);
}

float Vector3::magnitude() const
{
    return sqrtf(squaredlen());
}

vec3_t &Vector3::normalize()
{
    div(magnitude());
    return *this;
}

// should NOT be used
float Vector3::fmag() const
{
    return Maths::fSqrt(squaredlen());
}

float Vector3::ufmag() const
{
    return Maths::ufSqrt(squaredlen());
}

float Vector3::fmagxy() const
{
    return Maths::fSqrt(x * x + y * y);
}

float Vector3::ufmagxy() const
{
    return Maths::ufSqrt(x * x + y * y);
}

float Vector3::dist(const vec3_t &e) const
{
    vec3_t t;
    return dist(e, t);
}

float Vector3::dist(const vec3_t &e, vec3_t &t) const
{
    t = *this;
    t.sub(e);
    return t.magnitude();
}

float Vector3::distxy(const vec3_t &e) const
{
    float dx = e.x - x, dy = e.y - y;
    return sqrtf(dx * dx + dy * dy);
}

float Vector3::magnitudexy() const
{
    return sqrtf(x * x + y * y);
}

bool Vector3::reject(const vec3_t &o, float max) const
{
    return (
        x > o.x + max ||
        x < o.x - max ||
        y > o.y + max ||
        y < o.y - max
    );
}

vec3_t Vector3::cross(const vec3_t &a, const vec3_t &b)
{
    vec3_t retVec = vec3_t(0.0f, 0.0f, 0.0f);
    // retVec.x = (a.y * b.z - a.z * b.y);
    // retVec.y = (a.z * b.x - a.x * b.z);
    // retVec.z = (a.x * b.y - a.y * b.x);
    retVec = (a - b);
    return retVec;
}

float Vector3::cxy(const vec3_t &a)
{
    return x * a.y - y * a.x;
}

void Vector3::rotate_around_z(float angle)
{
    *this = vec3_t(cosf(angle) * x - sinf(angle) * y, cosf(angle) * y + sinf(angle) * x, z);
}

void Vector3::rotate_around_x(float angle)
{
    *this = vec3_t(x, cosf(angle) * y - sinf(angle) * z, cosf(angle) * z + sinf(angle) * y);
}

void Vector3::rotate_around_y(float angle)
{
    *this = vec3_t(cosf(angle) * x - sinf(angle) * z, y, cosf(angle) * z + sinf(angle) * x);
}

vec3_t &Vector3::rotate(float angle, const vec3_t &d)
{
    float c = cosf(angle), s = sinf(angle);
    return rotate(c, s, d);
}

vec3_t &Vector3::rotate(float c, float s, const vec3_t &d)
{
    *this = vec3_t(x * (d.x * d.x * (1 - c) + c) + y * (d.x * d.y * (1 - c) - d.z * s) + z * (d.x * d.z * (1 - c) + d.y * s),
                   x * (d.y * d.x * (1 - c) + d.z * s) + y * (d.y * d.y * (1 - c) + c) + z * (d.y * d.z * (1 - c) - d.x * s),
                   x * (d.x * d.z * (1 - c) - d.y * s) + y * (d.y * d.z * (1 - c) + d.x * s) + z * (d.z * d.z * (1 - c) + c));
    return *this;
}

void Vector3::orthogonal(const vec3_t &d)
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
* ==========
* Vector FLOAT 4
* ==========
*/
Vector4::Vector4(float _x, float _y, float _z, float _w)
    : x(_x), y(_y), z(_z), w(_w) 
{

}
 
// Vector4::Vector4(float _v)
//     : x(_v), y(_v), z(_v), w(_v) 
// {

// }

Vector4::Vector4(float* _v) 
    : x(_v[0]), y(_v[1]), z(_v[2]), w(_v[3]) 
{ 
    
}

Vector4::Vector4(const vec3_t &_v, float _a)
    : x(_v.x), y(_v.y), z(_v.z), w(_a) 
{
    
}

vec4_t Vector4::operator*(float _v)
{
    x *= _v;
    y *= _v;
    z *= _v;
    w *= _v;
    return *this;
}

vec4_t Vector4::operator/(float _v)
{
    x /= _v;
    y /= _v;
    z /= _v;
    w /= _v;
    return *this;
}

vec4_t Vector4::operator*(const vec4_t &_v)
{
    x *= _v.x;
    y *= _v.y;
    z *= _v.z;
    w *= _v.w;
    return *this;
}

vec4_t Vector4::operator/(const vec4_t &_v)
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
ivec3::ivec3(int _x, int _y, int _z)
    : x(_x), y(_y), z(_z)
{

}
 
ivec3::ivec3(const vec3_t &v)
    : x(int(v.x)), y(int(v.y)), z(int(v.z))
{

}

ivec3::~ivec3()
{

}

vec3_t ivec3::tovec() const
{
    return vec3_t((float)x, (float)y, (float)z);
}

int &ivec3::operator[](int i)
{
    return v[i];
}

int ivec3::operator[](int i) const
{
    return v[i];
}

bool ivec3::operator==(const ivec3_t &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

bool ivec3::operator!=(const ivec3_t &v) const
{
    return x != v.x || y != v.y || z != v.z;
}

ivec3_t &ivec3::mul(int n)
{
    x *= n;
    y *= n;
    z *= n;
    return *this;
}

ivec3_t &ivec3::div(int n)
{
    x /= n;
    y /= n;
    z /= n;
    return *this;
}

ivec3_t &ivec3::add(int n)
{
    x += n;
    y += n;
    z += n;
    return *this;
}

ivec3_t &ivec3::sub(int n)
{
    x -= n;
    y -= n;
    z -= n;
    return *this;
}

ivec3_t &ivec3::add(const ivec3_t &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

ivec3_t &ivec3::sub(const ivec3_t &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

ivec3_t &ivec3::mask(int n)
{
    x &= n;
    y &= n;
    z &= n;
    return *this;
}

ivec3_t &ivec3::cross(const ivec3_t &a, const ivec3_t &b)
{
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
    return *this;
}

int ivec3::dot(const ivec3_t &o) const
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
bvec3::bvec3(uchar x, uchar y, uchar z)
    : x(x), y(y), z(z)
{

}

bvec3::bvec3(const vec3_t &v)
    : x((uchar)((v.x + 1) * 255 / 2)), y((uchar)((v.y + 1) * 255 / 2)), z((uchar)((v.z + 1) * 255 / 2))
{

}

bvec3::~bvec3()
{

}

uchar &bvec3::operator[](int i)
{
    return v[i];
}

uchar bvec3::operator[](int i) const
{
    return v[i];
}

bool bvec3::operator==(const bvec3_t &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

bool bvec3::operator!=(const bvec3_t &v) const
{
    return x != v.x || y != v.y || z != v.z;
}

bool bvec3::iszero() const
{
    return x == 0 && y == 0 && z == 0;
}

vec3_t bvec3::tovec() const
{
    return vec3_t(
        x * (2.0f / 255.0f) - 1.0f, 
        y * (2.0f / 255.0f) - 1.0f, 
        z * (2.0f / 255.0f) - 1.0f
    );
}