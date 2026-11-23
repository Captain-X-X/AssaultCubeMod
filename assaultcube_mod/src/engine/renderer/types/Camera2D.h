#ifndef CAMERA2D_H
#define CAMERA2D_H

#include "pch.h"
#include "Viewport.h"

/*
* ==========
* 
* ==========
*/
class Camera2D
{
private:
    Vector3 m_origin;
    Vector3 m_rotation;
    Matrix4 m_viewMatrix;
    Viewport m_viewport;
    bool m_dirty;

public:
    Camera2D();
    ~Camera2D();

    void update();
    Matrix4& getViewMatrix();

    void setViewport(const Viewport& vp);
    void setPosition(const Vector3& origin);
    void setRotation(const Vector3& rotation);

    // getters
    Viewport& getViewport();
};

#endif