#ifndef CMEMATRIXMATHS_H
#define CMEMATRIXMATHS_H
#include "vector_maths.h"

typedef struct quaternion_t {
public:
    float x,y,z,w;

} quaternion, quat_t;

/*
* ==========
* vector of FLOAT 3-3
* ==========
*/
typedef struct mat3_t {
    float elements[9];

    mat3_t(float _v = 1.0f);

    static mat3_t identity() {
        return mat3_t(1.0f);
    }
} Mat3, Matrix3x3;

/*
* ==========
* vector of FLOAT 4-4
* ==========
*/
typedef struct mat4_t {
    float v[16];

    float operator[](int i) const;
    float &operator[](int i);

    #define ROTVEC(A, B) \
    { \
        float a = A, b = B; \
        A = a*c + b*s; \
        B = b*c - a*s; \
    }

    void rotate_around_x(float angle);
    void rotate_around_y(float angle);
    void rotate_around_z(float angle);
    #undef ROTVEC

    #define MULMAT(row, col) \
       v[col + row] = x[row]*y[col] + x[row + 4]*y[col + 1] + x[row + 8]*y[col + 2] + x[row + 12]*y[col + 3];

    template<class XT, class YT>
    void mul(const XT x[16], const YT y[16])
    {
        MULMAT(0, 0); MULMAT(1, 0); MULMAT(2, 0); MULMAT(3, 0);
        MULMAT(0, 4); MULMAT(1, 4); MULMAT(2, 4); MULMAT(3, 4);
        MULMAT(0, 8); MULMAT(1, 8); MULMAT(2, 8); MULMAT(3, 8);
        MULMAT(0, 12); MULMAT(1, 12); MULMAT(2, 12); MULMAT(3, 12);
    }

    #undef MULMAT

    void mul(const mat4_t &x, const mat4_t &y);
    void identity();
    void translate(float x, float y, float z);
    void translate(const vec3_t &o);
    void scale(float x, float y, float z);
    void projective();
    void invertnormal(vec3_t& dir) const;
    void invertvertex(vec3_t& pos) const;
    float transformx(const vec3_t& p) const;
    float transformy(const vec3_t& p) const;
    float transformz(const vec3_t& p) const;
    float transformw(const vec3_t& p) const;
    void transform(const vec3_t& in, vec4 &out) const;
    vec3_t gettranslation() const;
    float determinant() const;
    void adjoint(const mat4_t &m);
    bool invert(const mat4_t &m, float mindet = 1.0e-10f);
} Matrix4x4, glmatrixf;

#endif