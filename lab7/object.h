#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "geometry.h"

class Polygon
{
public:
    std::vector<Point3D> points;

    Polygon(const std::vector<Point3D> &points = {});
    void transform(const Matrix4x4 &matrix);
};

class Polyhedron
{
public:
    std::vector<Polygon> polygons;

    Polyhedron(const std::vector<Polygon> &polygons = {});
    void transform(const Matrix4x4 &matrix);
    Point3D getCenter() const;
};

#endif