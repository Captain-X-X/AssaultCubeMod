#ifndef CAMERA_H
#define CAMERA_H
#include "maths/vector_maths.h"
#include "maths/matrix_maths.h"

class Camera3D
{
private:
    Vector3 m_viewAngles;
    // Vector3 dir;
    // Vector3 forward;
    // Vector3 left;
    Vector3 m_origin;
    Matrix4 m_viewMatrix;

public:
    Camera3D();
    ~Camera3D();

    void create(const Vector3& default_angles = Vector3(0.0f, 0.0f, 0.0f));
    void setViewAngles(const Vector3& newAngles);
    void followPosition(const Vector3& pos);
    Matrix4& getViewMatrixRaw() ; // calculate and then return view matrix
};

#endif