#ifndef CAMERA_H
#define CAMERA_H
#include "maths/vector_maths.h"
#include "maths/matrix_maths.h"

class Camera
{
public:
    Vector3 viewAngles;
    // Vector3 dir;
    // Vector3 forward;
    // Vector3 left;
    Vector3 origin;
    Matrix4x4 viewMatrix;

public:
    Camera();
    ~Camera();

    void create(const Vector3& default_angles = Vector3(0.0f, 0.0f, 0.0f));
    void setViewAngles(const Vector3& newAngles);
    void followPosition(const Vector3& pos);
    Matrix4x4& getViewMatrixRaw() ; // calculate and then return view matrix
};

#endif