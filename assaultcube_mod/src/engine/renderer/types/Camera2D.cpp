#include "pch.h"
#include "Camera2D.h"

/*
* ==========
* 
* ==========
*/
Camera2D::Camera2D()
    : m_origin(Vector3(0.0f, 0.0f, 0.0f)), m_rotation(Vector3(0.0f, 0.0f, 0.0f))
{
    m_viewMatrix = Matrix4::Identity();

    m_viewport.x = 0;
    m_viewport.y = 0;
    m_viewport.width = 0;
    m_viewport.height = 0;
    m_dirty = true;
}

Camera2D::~Camera2D()
{

}

void Camera2D::update()
{
    if (m_dirty) {
        m_viewMatrix = (Matrix4::Ortho(
            (float)m_viewport.x,        
            (float)m_viewport.width,
            (float)m_viewport.height,   
            (float)m_viewport.y,
            -1.0f, 1.0f
        ));
        m_dirty = false;
    }
}

Matrix4& Camera2D::getViewMatrix()
{
    if (m_dirty) {
        update();
    }
    return m_viewMatrix;
}

void Camera2D::setViewport(const Viewport& vp)
{
    m_viewport = vp;
}

void Camera2D::setPosition(const Vector3& origin)
{
    m_origin = origin;
}

void Camera2D::setRotation(const Vector3& rotation)
{
    m_rotation = rotation;
}

Viewport& Camera2D::getViewport() 
{ 
    return m_viewport; 
}