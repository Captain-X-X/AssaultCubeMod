#ifndef MATRIXMATHS_H
#define MATRIXMATHS_H
#include "vector_maths.h"

/*
* ==========
* Matrix 3x3
* ==========
*/
class Matrix3 
{
public:
    float v[9];

public:
    Matrix3();
    ~Matrix3();

    static Matrix3 Identity();

    // getters
    float* getValue() { return v; }
};
typedef Matrix3 Mat3;
typedef Matrix3 mat3;
typedef Matrix3 mat3_t;
typedef Matrix3 glmatrix3f;

/*
* ==========
* Matrix 4x4
* ==========
*/
class Matrix4
{
public:
    float v[16];

public:
    Matrix4();
    ~Matrix4();

    float operator[](int i) const;
    float &operator[](int i);

    Matrix4 operator*(const Matrix4& mat);
    Matrix4 operator*=(const Matrix4& mat);

    #define ROTVEC(A, B) { \
        float a = A, b = B; \
        A = a*c + b*s; \
        B = b*c - a*s; \
    }
    void rotate_around_x(float angle);
    void rotate_around_y(float angle);
    void rotate_around_z(float angle);
    #undef ROTVEC

    #define MULMAT(row, col) v[col + row] = x[row]*y[col] + x[row + 4]*y[col + 1] + x[row + 8]*y[col + 2] + x[row + 12]*y[col + 3];
    template<class XT, class YT>
    void mul(const XT x[16], const YT y[16]) {
        MULMAT(0, 0); MULMAT(1, 0); MULMAT(2, 0); MULMAT(3, 0);
        MULMAT(0, 4); MULMAT(1, 4); MULMAT(2, 4); MULMAT(3, 4);
        MULMAT(0, 8); MULMAT(1, 8); MULMAT(2, 8); MULMAT(3, 8);
        MULMAT(0, 12); MULMAT(1, 12); MULMAT(2, 12); MULMAT(3, 12);
    }
    #undef MULMAT

    void mul(const Matrix4 &x, const Matrix4 &y);
    void identity();
    void translate(float x, float y, float z);
    void translate(const vec3 &o);
    void scale(float x, float y, float z);
    void projective();
    void invertnormal(vec3& dir) const;
    void invertvertex(vec3& pos) const;
    float transformx(const vec3& p) const;
    float transformy(const vec3& p) const;
    float transformz(const vec3& p) const;
    float transformw(const vec3& p) const;
    void transform(const vec3& in, vec4 &out) const;
    vec3 gettranslation() const;
    float determinant() const;
    void adjoint(const Matrix4 &m);
    bool invert(const Matrix4 &m, float mindet = 1.0e-10f);
    
    static Matrix4 Identity();
    static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far);
};
typedef Matrix4 Mat4;
typedef Matrix4 mat4;
typedef Matrix4 mat4_t;
typedef Matrix4 glmatrixf;

/*
* ==========
* 
* ==========
*/
class Quaternion 
{
public:
    float x, y, z, w;
public:

};
typedef Quaternion quaternion, quaternion_t, quat_t;

#endif