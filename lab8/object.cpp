#include "object.h"

Polygon::Polygon(const std::vector<Point3D> &points) : points(points) {}

void Polygon::transform(const Matrix4x4 &matrix)
{
    for (auto &point : points)
    {
        point = matrix.transform(point);
    }
}

Polyhedron::Polyhedron(const std::vector<Polygon> &polygons) : polygons(polygons) {}

void Polyhedron::transform(const Matrix4x4 &matrix)
{
    for (auto &polygon : polygons)
    {
        polygon.transform(matrix);
    }
}

Point3D Polyhedron::getCenter() const
{
    Point3D center;
    int count = 0;
    for (const auto &polygon : polygons)
    {
        for (const auto &point : polygon.points)
        {
            center = center + point;
            count++;
        }
    }
    if (count > 0)
    {
        center = center * (1.0 / count);
    }
    return center;
}

void Polygon::calculateNormal()
{
    if (points.size() < 3)
    {
        normal = Vector3D(0, 0, 0);
        return;
    }
    Vector3D a = points[1] - points[0];
    Vector3D b = points[2] - points[0];
    normal = a.cross(b).normalized();
}

void Polyhedron::calculateNormals()
{
    for (auto &polygon : polygons)
    {
        polygon.calculateNormal();
    }
}