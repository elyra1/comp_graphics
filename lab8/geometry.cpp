#include "geometry.h"

Point3D::Point3D(double x, double y, double z, double w)
    : x(x), y(y), z(z), w(w) {}

Point3D Point3D::operator+(const Point3D &other) const
{
    return Point3D(x + other.x, y + other.y, z + other.z);
}

Point3D Point3D::operator-(const Point3D &other) const
{
    return Point3D(x - other.x, y - other.y, z - other.z);
}

Point3D Point3D::operator*(double scalar) const
{
    return Point3D(x * scalar, y * scalar, z * scalar);
}

double Point3D::dot(const Point3D &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

Point3D Point3D::cross(const Point3D &other) const
{
    return Point3D(y * other.z - z * other.y,
                   z * other.x - x * other.z,
                   x * other.y - y * other.x, 0);
}

double Point3D::length() const
{
    return std::sqrt(dot(*this));
}

void Point3D::normalize()
{
    double len = length();
    if (len > 0)
    {
        x /= len;
        y /= len;
        z /= len;
    }
}

Point3D Point3D::normalized() const
{
    Point3D result = *this;
    result.normalize();
    return result;
}

Matrix4x4::Matrix4x4()
{
    identity();
}

void Matrix4x4::identity()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0 : 0.0;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const
{
    Matrix4x4 result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

Point3D Matrix4x4::transform(const Point3D &point) const
{
    double x = m[0][0] * point.x + m[0][1] * point.y + m[0][2] * point.z + m[0][3] * point.w;
    double y = m[1][0] * point.x + m[1][1] * point.y + m[1][2] * point.z + m[1][3] * point.w;
    double z = m[2][0] * point.x + m[2][1] * point.y + m[2][2] * point.z + m[2][3] * point.w;
    double w = m[3][0] * point.x + m[3][1] * point.y + m[3][2] * point.z + m[3][3] * point.w;

    if (w != 0 && w != 1)
    {
        x /= w;
        y /= w;
        z /= w;
    }
    return Point3D(x, y, z, w);
}

Matrix4x4 createTranslationMatrix(double tx, double ty, double tz)
{
    Matrix4x4 mat;
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Matrix4x4 createScaleMatrix(double sx, double sy, double sz)
{
    Matrix4x4 mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

Matrix4x4 createRotationXMatrix(double angle)
{
    double rad = angle * M_PI / 180.0;
    Matrix4x4 mat;
    mat.m[1][1] = cos(rad);
    mat.m[1][2] = -sin(rad);
    mat.m[2][1] = sin(rad);
    mat.m[2][2] = cos(rad);
    return mat;
}

Matrix4x4 createRotationYMatrix(double angle)
{
    double rad = angle * M_PI / 180.0;
    Matrix4x4 mat;
    mat.m[0][0] = cos(rad);
    mat.m[0][2] = sin(rad);
    mat.m[2][0] = -sin(rad);
    mat.m[2][2] = cos(rad);
    return mat;
}

Matrix4x4 createRotationZMatrix(double angle)
{
    double rad = angle * M_PI / 180.0;
    Matrix4x4 mat;
    mat.m[0][0] = cos(rad);
    mat.m[0][1] = -sin(rad);
    mat.m[1][0] = sin(rad);
    mat.m[1][1] = cos(rad);
    return mat;
}

Matrix4x4 createArbitraryRotationMatrix(const Point3D &p1, const Point3D &p2, double angle)
{
    Point3D axis = p2 - p1;
    double len = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    double l = axis.x / len;
    double m = axis.y / len;
    double n = axis.z / len;

    double rad = angle * M_PI / 180.0;
    double cosA = cos(rad);
    double sinA = sin(rad);

    Matrix4x4 T = createTranslationMatrix(-p1.x, -p1.y, -p1.z);

    Matrix4x4 R;
    R.m[0][0] = l * l + (1 - l * l) * cosA;
    R.m[0][1] = l * m * (1 - cosA) - n * sinA;
    R.m[0][2] = l * n * (1 - cosA) + m * sinA;
    R.m[1][0] = l * m * (1 - cosA) + n * sinA;
    R.m[1][1] = m * m + (1 - m * m) * cosA;
    R.m[1][2] = m * n * (1 - cosA) - l * sinA;
    R.m[2][0] = l * n * (1 - cosA) - m * sinA;
    R.m[2][1] = m * n * (1 - cosA) + l * sinA;
    R.m[2][2] = n * n + (1 - n * n) * cosA;

    Matrix4x4 T_inv = createTranslationMatrix(p1.x, p1.y, p1.z);

    return T_inv * R * T;
}

Matrix4x4 createReflectionMatrix(ReflectionPlane plane)
{
    Matrix4x4 mat;
    switch (plane)
    {
    case ReflectionPlane::X:
        mat.m[0][0] = -1;
        break;
    case ReflectionPlane::Y:
        mat.m[1][1] = -1;
        break;
    case ReflectionPlane::Z:
        mat.m[2][2] = -1;
        break;
    }
    return mat;
}

Matrix4x4 createPerspectiveMatrix(double fovY, double aspect, double zNear, double zFar)
{
    double tanHalfFovy = tan(fovY * M_PI / 360.0);
    Matrix4x4 result;
    result.identity();
    result.m[0][0] = 1.0 / (aspect * tanHalfFovy);
    result.m[1][1] = 1.0 / (tanHalfFovy);
    result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
    result.m[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
    result.m[3][2] = -1.0;
    result.m[3][3] = 0.0;
    return result;
}

Matrix4x4 createAxonometricMatrix(double left, double right, double bottom, double top, double zNear, double zFar)
{
    Matrix4x4 result;
    result.identity();
    result.m[0][0] = 2.0 / (right - left);
    result.m[1][1] = 2.0 / (top - bottom);
    result.m[2][2] = -2.0 / (zFar - zNear);
    result.m[0][3] = -(right + left) / (right - left);
    result.m[1][3] = -(top + bottom) / (top - bottom);
    result.m[2][3] = -(zFar + zNear) / (zFar - zNear);
    return result;
}

Matrix4x4 lookAt(const Point3D& eye, const Point3D& center, const Point3D& up)
{
    Point3D f = (center - eye).normalized();
    Point3D s = f.cross(up).normalized();
    Point3D u = s.cross(f);

    Matrix4x4 result;
    result.identity();

    result.m[0][0] = s.x;
    result.m[0][1] = s.y;
    result.m[0][2] = s.z;
    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;
    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;
    result.m[0][3] = -s.dot(eye);
    result.m[1][3] = -u.dot(eye);
    result.m[2][3] = f.dot(eye);

    return result;
}
