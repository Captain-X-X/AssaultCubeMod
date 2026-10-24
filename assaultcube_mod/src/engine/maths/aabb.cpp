#include "aabb.h"
#include "maths_func.h"

void aabb_s::addMin(const vec3_t& v)
{
    min.x = Maths::min(min.x, v.x);
    min.y = Maths::min(min.y, v.y);
    min.z = Maths::min(min.z, v.z);
}

void aabb_s::addMax(const vec3_t& v)
{
    max.x = Maths::max(max.x, v.x);
    max.y = Maths::max(max.y, v.y);
    max.z = Maths::max(max.z, v.z);
}

void aabb_s::add(const vec3_t& v)
{
    addMin(v);
    addMax(v);
}

void aabb_s::add(const aabb_s& b)
{
    addMin(b.min);
    addMax(b.max);
}