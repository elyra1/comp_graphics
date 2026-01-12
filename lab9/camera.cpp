#include "camera.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera()
{
    position = Point3D(0, 0, 5);
    target = Point3D(0, 0, 0);
    up = Point3D(0, 1, 0);
    fov = 45.0f;
    aspectRatio = 800.0f / 600.0f;
    zNear = 0.1f;
    zFar = 100.0f;
}

Matrix4x4 Camera::getViewMatrix()
{
    return lookAt(position, target, up);
}

Matrix4x4 Camera::getProjectionMatrix(bool perspective)
{
    if (perspective)
    {
        return createPerspectiveMatrix(fov, aspectRatio, zNear, zFar);
    }
    else
    {
        return createAxonometricMatrix(-2.5, 2.5, -2.5, 2.5, -100.0, 100.0);
    }
}

void Camera::orbit(float deltaX, float deltaY)
{
    Point3D dir = position - target;
    float radius = dir.length();
    float theta = atan2(dir.x, dir.z);
    float phi = acos(dir.y / radius);

    theta += deltaX * 0.01f;
    phi -= deltaY * 0.01f;

    phi = std::max(0.1f, std::min((float)M_PI - 0.1f, phi));

    position.x = target.x + radius * sin(phi) * sin(theta);
    position.y = target.y + radius * cos(phi);
    position.z = target.z + radius * sin(phi) * cos(theta);
}
