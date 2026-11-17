#include "pch.h"
#include "camera.h"

Camera::Camera()
    : viewAngles(Vector3(0.0f, 0.0f, 0.0f))
{
    viewMatrix.identity();
}

Camera::~Camera()
{
}

void Camera::create(const Vector3& default_angles)
{
    viewAngles = default_angles;
}

void Camera::setViewAngles(const Vector3& newAngles)
{
    viewAngles = newAngles;
}

void Camera::followPosition(const Vector3& pos)
{
    origin = pos;
}

Matrix4x4& Camera::getViewMatrixRaw()
{
    viewMatrix.rotate_around_z(viewAngles.z);
    viewMatrix.rotate_around_x(viewAngles.x);
    viewMatrix.rotate_around_y(viewAngles.y);

    // RH -> Quake Z-up/LH conversion
    viewMatrix.rotate_around_x(ToRadians(-90.0f));
    viewMatrix.scale(1.0f, -1.0f, 1.0f);

    viewMatrix.translate(Vector3(
            -origin.x,
            -origin.y,
            -origin.z
        )
    );

    return viewMatrix;
}