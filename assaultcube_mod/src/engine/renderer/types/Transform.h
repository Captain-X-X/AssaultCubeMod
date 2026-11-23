/*
* ==========
* rendering transform matrix
* ==========
*/
#ifndef RENDERTRANSFORM_H
#define RENDERTRANSFORM_H

#include "maths/vector_maths.h"

/*
* ==========
* rendering transforms
* ==========
*/
struct Transform
{
    Vector3 origin;
    Vector3 rotation;
};

/*
* ==========
* this is for models that need transformations on loading
* ==========
*/
struct TransformRaw
{
    Vector3 origin;
    Vector3 rotation;
    Vector3 scale;

    inline Transform getTransform() const { 
        Transform t; 
        t.origin = origin; 
        t.rotation = rotation; 
        return t; 
    }
};

#endif