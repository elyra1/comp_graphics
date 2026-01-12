#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "geometry.h"

class Polygon
{
public:
    std::vector<Point3D> points;
    Vector3D normal;

    Polygon(const std::vector<Point3D> &points = {});
    void transform(const Matrix4x4 &matrix);
    void calculateNormal();
};

class Polyhedron
{
public:
    std::vector<Polygon> polygons;

    Polyhedron(const std::vector<Polygon> &polygons = {});
    void transform(const Matrix4x4 &matrix);
    Point3D getCenter() const;
    void calculateNormals();
};

class SceneObject
{
public:
    Polyhedron polyhedron;
    Matrix4x4 currentTransformation;

    SceneObject(const Polyhedron& p = Polyhedron(), const Matrix4x4& t = Matrix4x4())
        : polyhedron(p), currentTransformation(t) {}
};

#endif