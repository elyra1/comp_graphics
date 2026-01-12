#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Point3D
{
public:
    double x, y, z, w;

    Point3D(double x = 0, double y = 0, double z = 0, double w = 1);

    Point3D operator+(const Point3D &other) const;
    Point3D operator-(const Point3D &other) const;
    Point3D operator*(double scalar) const;
};

class Matrix4x4
{
public:
    double m[4][4];

    Matrix4x4();
    void identity();
    Matrix4x4 operator*(const Matrix4x4 &other) const;
    Point3D transform(const Point3D &point) const;
};

Matrix4x4 createTranslationMatrix(double tx, double ty, double tz);
Matrix4x4 createScaleMatrix(double sx, double sy, double sz);
Matrix4x4 createRotationXMatrix(double angle);
Matrix4x4 createRotationYMatrix(double angle);
Matrix4x4 createRotationZMatrix(double angle);
Matrix4x4 createArbitraryRotationMatrix(const Point3D &p1, const Point3D &p2, double angle);
enum class ReflectionPlane
{
    X,
    Y,
    Z
};

Matrix4x4 createReflectionMatrix(ReflectionPlane plane);

Matrix4x4 createPerspectiveMatrix(double fovY, double aspect, double zNear, double zFar);
Matrix4x4 createAxonometricMatrix(double left, double right, double bottom, double top, double zNear, double zFar);

#endif
