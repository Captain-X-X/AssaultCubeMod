#include "pch.h"
#include "Camera3D.h"

Camera3D::Camera3D()
    : m_viewAngles(Vector3(0.0f, 0.0f, 0.0f))
{
    m_viewMatrix.identity();
}

Camera3D::~Camera3D()
{
}

void Camera3D::create(const Vector3& default_angles)
{
    m_viewAngles = default_angles;
}

void Camera3D::setViewAngles(const Vector3& newAngles)
{
    m_viewAngles = newAngles;
}

void Camera3D::followPosition(const Vector3& pos)
{
    m_origin = pos;
}

Matrix4& Camera3D::getViewMatrixRaw()
{
    m_viewMatrix.rotate_around_z(m_viewAngles.z);
    m_viewMatrix.rotate_around_x(m_viewAngles.x);
    m_viewMatrix.rotate_around_y(m_viewAngles.y);

    // RH -> Quake Z-up/LH conversion
    m_viewMatrix.rotate_around_x(ToRadians(-90.0f));
    m_viewMatrix.scale(1.0f, -1.0f, 1.0f);

    m_viewMatrix.translate(Vector3(
            -m_origin.x,
            -m_origin.y,
            -m_origin.z
        )
    );

    return m_viewMatrix;
}