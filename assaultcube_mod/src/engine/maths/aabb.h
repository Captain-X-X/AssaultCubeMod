#ifndef CMEAABB_H
#define CMEAABB_H
#include "vector_maths.h"

struct aabb_s {
    vec3_t min;
    vec3_t max;

    void addMin(const vec3_t &v);
    void addMax(const vec3_t &v);
    void add(const vec3_t &v);
    void add(const aabb_s &b);
};

#endif