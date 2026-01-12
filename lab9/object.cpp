#include "object.h"
#include <map>

Polygon::Polygon(const std::vector<Point3D> &points,
                 const std::vector<TextureCoord> &textureCoords)
    : points(points), textureCoords(textureCoords) {}

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

void Polyhedron::calculateVertexNormals()
{
    // Карта для хранения суммы нормалей для каждой уникальной вершины
    std::map<Point3D, Vector3D> vertexNormalSum;

    // 1. Накапливаем нормали для каждой вершины
    for (const auto &polygon : polygons)
    {
        for (const auto &point : polygon.points)
        {
            // Добавляем нормаль грани к сумме для данной вершины
            vertexNormalSum[point] = vertexNormalSum[point] + polygon.normal;
        }
    }

    // 2. Нормализуем суммарные нормали
    for (auto &pair : vertexNormalSum)
    {
        pair.second.normalize();
    }

    // 3. Присваиваем вычисленные нормали вершинам каждого полигона
    for (auto &polygon : polygons)
    {
        polygon.vertexNormals.clear();
        polygon.vertexNormals.reserve(polygon.points.size());
        for (const auto &point : polygon.points)
        {
            polygon.vertexNormals.push_back(vertexNormalSum[point]);
        }
    }
}