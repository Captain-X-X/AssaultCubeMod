#include "matrix_maths.h"
#include "maths_func.h"
#include <cmath>
#include <memory.h>

/*
    MMaths: mat4_t
*/
float mat4_t::operator[](int i) const { return v[i]; }
float& mat4_t::operator[](int i) { return v[i]; }

#define ROTVEC(A, B) \
{ \
    float a = A, b = B; \
    A = a*c + b*s; \
    B = b*c - a*s; \
}

void mat4_t::rotate_around_x(float angle)
{
    float c = cosf(angle), s = sinf(angle);
    ROTVEC(v[4], v[8]);
    ROTVEC(v[5], v[9]);
    ROTVEC(v[6], v[10]);
}

void mat4_t::rotate_around_y(float angle)
{
    float c = cosf(angle), s = sinf(angle);
    ROTVEC(v[8], v[0]);
    ROTVEC(v[9], v[1]);
    ROTVEC(v[10], v[2]);
}

void mat4_t::rotate_around_z(float angle)
{
    float c = cosf(angle), s = sinf(angle);
    ROTVEC(v[0], v[4]);
    ROTVEC(v[1], v[5]);
    ROTVEC(v[2], v[6]);
}

#undef ROTVEC

void mat4_t::mul(const mat4_t &x, const mat4_t &y)
{
    mul(x.v, y.v);
}

void mat4_t::identity()
{
    static const float m[16] =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    memcpy(v, m, sizeof(v));
}

void mat4_t::translate(float x, float y, float z)
{
    v[12] += x;
    v[13] += y;
    v[14] += z;
}

void mat4_t::translate(const vec3_t &o)
{
    translate(o.x, o.y, o.z);
}

void mat4_t::scale(float x, float y, float z)
{
    v[0] *= x; v[1] *= x; v[2] *= x; v[3] *= x;
    v[4] *= y; v[5] *= y; v[6] *= y; v[7] *= y;
    v[8] *= z; v[9] *= z; v[10] *= z; v[11] *= z;
}

void mat4_t::projective()
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            v[i + j*4] = 0.5f*(v[i + j*4] + v[3 + j*4]);
        }
    }
}

void mat4_t::invertnormal(vec3_t& dir) const
{
    vec3_t n(dir);
    dir.x = n.x*v[0] + n.y*v[1] + n.z*v[2];
    dir.y = n.x*v[4] + n.y*v[5] + n.z*v[6];
    dir.z = n.x*v[8] + n.y*v[9] + n.z*v[10];
}

void mat4_t::invertvertex(vec3_t& pos) const
{
    vec3_t p(pos);
    p.x -= v[12];
    p.y -= v[13];
    p.z -= v[14];
    pos.x = p.x*v[0] + p.y*v[1] + p.z*v[2];
    pos.y = p.x*v[4] + p.y*v[5] + p.z*v[6];
    pos.z = p.x*v[8] + p.y*v[9] + p.z*v[10];
}

float mat4_t::transformx(const vec3_t& p) const
{
    return p.x*v[0] + p.y*v[4] + p.z*v[8] + v[12];
}

float mat4_t::transformy(const vec3_t& p) const
{
    return p.x*v[1] + p.y*v[5] + p.z*v[9] + v[13];
}

float mat4_t::transformz(const vec3_t& p) const
{
    return p.x*v[2] + p.y*v[6] + p.z*v[10] + v[14];
}

float mat4_t::transformw(const vec3_t& p) const
{
    return p.x*v[3] + p.y*v[7] + p.z*v[11] + v[15];
}

void mat4_t::transform(const vec3_t& in, vec4 &out) const
{
    out.x = transformx(in);
    out.y = transformy(in);
    out.z = transformz(in);
    out.w = transformw(in);
}

vec3_t mat4_t::gettranslation() const
{
    return vec(v[12], v[13], v[14]);
}

float mat4_t::determinant() const
{
    float a1 = v[0], a2 = v[1], a3 = v[2], a4 = v[3],
          b1 = v[4], b2 = v[5], b3 = v[6], b4 = v[7],
          c1 = v[8], c2 = v[9], c3 = v[10], c4 = v[11],
          d1 = v[12], d2 = v[13], d3 = v[14], d4 = v[15];

    return a1 * Maths::det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
         - b1 * Maths::det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
         + c1 * Maths::det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
         - d1 * Maths::det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

void mat4_t::adjoint(const mat4_t &m)
{
    float a1 = m.v[0], a2 = m.v[1], a3 = m.v[2], a4 = m.v[3],
          b1 = m.v[4], b2 = m.v[5], b3 = m.v[6], b4 = m.v[7],
          c1 = m.v[8], c2 = m.v[9], c3 = m.v[10], c4 = m.v[11],
          d1 = m.v[12], d2 = m.v[13], d3 = m.v[14], d4 = m.v[15];

    v[0]  =  Maths::det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    v[1]  = -Maths::det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    v[2]  =  Maths::det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    v[3]  = -Maths::det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    v[4]  = -Maths::det3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    v[5]  =  Maths::det3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    v[6]  = -Maths::det3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    v[7]  =  Maths::det3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

    v[8]  =  Maths::det3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    v[9]  = -Maths::det3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    v[10] =  Maths::det3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    v[11] = -Maths::det3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

    v[12] = -Maths::det3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    v[13] =  Maths::det3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    v[14] = -Maths::det3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    v[15] =  Maths::det3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

bool mat4_t::invert(const mat4_t &m, float mindet)
{
    float a1 = m.v[0], b1 = m.v[4], c1 = m.v[8], d1 = m.v[12];
    adjoint(m);
    float det = a1*v[0] + b1*v[1] + c1*v[2] + d1*v[3]; // float det = m.determinant();
    if(fabs(det) < mindet) return false;
    float invdet = 1/det;
    for (int i = 0; i < 16; i++) v[i] *= invdet;
    return true;
}


/*
    MMaths: Functions
*/
static quat_t ToQuat(const vec3_t& _v)
{
    quat_t q;
    float cx = cosf(_v.x*0.5f);
    float sx = sinf(_v.x*0.5f);
    float cy = cosf(_v.y*0.5f);
    float sy = sinf(_v.y*0.5f);
    float cz = cosf(_v.z*0.5f);
    float sz = sinf(_v.z*0.5f);
    q.w = cx * cy * cz + sx * sy * sz;
    q.x = sx * cy * cz - cx * sy * sz;
    q.y = cx * sy * cz + sx * cy * sz;
    q.z = cx * cy * sz - sx * sy * cz;
    return q;
}

