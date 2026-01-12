#include "shapes.h"

Polyhedron createPolyhedronFromVerticesAndFaces(const std::vector<Point3D> &vertices, const std::vector<std::vector<int>> &faces)
{
    std::vector<Polygon> polygons;
    for (const auto &face : faces)
    {
        std::vector<Point3D> polygonPoints;
        for (int idx : face)
        {
            polygonPoints.push_back(vertices[idx]);
        }
        polygons.push_back(Polygon(polygonPoints));
    }
    return Polyhedron(polygons);
}

Polyhedron createTetrahedron()
{
    std::vector<Point3D> vertices = {
        Point3D(1, 1, 1),
        Point3D(-1, -1, 1),
        Point3D(-1, 1, -1),
        Point3D(1, -1, -1)};

    std::vector<std::vector<int>> faces = {
        {0, 1, 2},
        {0, 3, 1},
        {0, 2, 3},
        {1, 3, 2}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}

Polyhedron createHexahedron()
{
    std::vector<Point3D> vertices = {
        Point3D(-1, -1, -1),
        Point3D(1, -1, -1),
        Point3D(1, 1, -1),
        Point3D(-1, 1, -1),
        Point3D(-1, -1, 1),
        Point3D(1, -1, 1),
        Point3D(1, 1, 1),
        Point3D(-1, 1, 1),
    };

    std::vector<std::vector<int>> faces = {
        {0, 1, 2, 3},
        {7, 6, 5, 4},
        {0, 1, 5, 4},
        {6, 7, 3, 2},
        {0, 3, 7, 4},
        {1, 2, 6, 5},
    };

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}

Polyhedron createOctahedron()
{
    std::vector<Point3D> vertices = {
        Point3D(1, 0, 0),
        Point3D(-1, 0, 0),
        Point3D(0, 1, 0),
        Point3D(0, -1, 0),
        Point3D(0, 0, 1),
        Point3D(0, 0, -1)};

    std::vector<std::vector<int>> faces = {
        {0, 4, 2},
        {0, 3, 4},
        {0, 5, 3},
        {0, 2, 5},
        {1, 2, 4},
        {1, 4, 3},
        {1, 3, 5},
        {1, 5, 2}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}
Polyhedron createIcosahedron()
{
    double phi = (1.0 + sqrt(5.0)) / 2.0;

    std::vector<Point3D> vertices = {
        Point3D(-1, phi, 0), Point3D(1, phi, 0), Point3D(-1, -phi, 0), Point3D(1, -phi, 0),
        Point3D(0, -1, phi), Point3D(0, 1, phi), Point3D(0, -1, -phi), Point3D(0, 1, -phi),
        Point3D(phi, 0, -1), Point3D(phi, 0, 1), Point3D(-phi, 0, -1), Point3D(-phi, 0, 1)};

    for (auto &v : vertices)
    {
        v = v * 0.5;
    }

    std::vector<std::vector<int>> faces = {
        {0, 5, 11}, {0, 1, 5}, {0, 7, 1}, {0, 10, 7}, {0, 11, 10}, {1, 9, 5}, {5, 4, 11}, {11, 2, 10}, {10, 6, 7}, {7, 8, 1}, {3, 4, 9}, {3, 2, 4}, {3, 6, 2}, {3, 8, 6}, {3, 9, 8}, {4, 5, 9}, {2, 11, 4}, {6, 10, 2}, {8, 7, 6}, {9, 1, 8}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}
Polyhedron createDodecahedron()
{
    double phi = (1.0 + sqrt(5.0)) / 2.0;
    double invPhi = 1.0 / phi;

    std::vector<Point3D> vertices = {
        Point3D(1, 1, 1),
        Point3D(1, 1, -1),
        Point3D(1, -1, 1),
        Point3D(1, -1, -1),
        Point3D(-1, 1, 1),
        Point3D(-1, 1, -1),
        Point3D(-1, -1, 1),
        Point3D(-1, -1, -1),

        Point3D(0, phi, invPhi),
        Point3D(0, phi, -invPhi),
        Point3D(0, -phi, invPhi),
        Point3D(0, -phi, -invPhi),

        Point3D(invPhi, 0, phi),
        Point3D(-invPhi, 0, phi),
        Point3D(invPhi, 0, -phi),
        Point3D(-invPhi, 0, -phi),

        Point3D(phi, invPhi, 0),
        Point3D(-phi, invPhi, 0),
        Point3D(phi, -invPhi, 0),
        Point3D(-phi, -invPhi, 0)};

    // Масштабируем для лучшего отображения
    for (auto &v : vertices)
    {
        v = v * 0.4;
    }

    std::vector<std::vector<int>> faces = {
        {0, 12, 13, 4, 8},
        {0, 16, 18, 2, 12},
        {0, 8, 9, 1, 16},

        {1, 14, 3, 18, 16},
        {1, 9, 5, 15, 14},

        {2, 10, 6, 13, 12},
        {2, 18, 3, 11, 10},

        {3, 14, 15, 7, 11},

        {4, 17, 5, 9, 8},
        {4, 13, 6, 19, 17},

        {5, 17, 19, 7, 15},

        {6, 10, 11, 7, 19}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}