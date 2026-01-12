#ifndef CAMERA_H
#define CAMERA_H

#include "geometry.h"

class Camera
{
public:
    Point3D position;
    Point3D target;
    Point3D up;
    float fov;
    float aspectRatio;
    float zNear;
    float zFar;

    Camera();

    Matrix4x4 getViewMatrix();
    Matrix4x4 getProjectionMatrix(bool perspective);
    void orbit(float deltaX, float deltaY);
};

#endif
