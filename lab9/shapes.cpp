#include "shapes.h"
#include <vector>

Polyhedron createFigureOfRevolution(const std::vector<Point3D> &generatingPoints, char axis, int steps)
{
    std::vector<Point3D> vertices;
    double angle;
    Matrix4x4 rotationMatrix;

    for (int i = 0; i < steps; ++i)
    {
        angle = i * 2.0 * M_PI / steps;
        switch (axis)
        {
        case 'x':
            rotationMatrix = createRotationXMatrix(angle * 180.0 / M_PI);
            break;
        case 'y':
            rotationMatrix = createRotationYMatrix(angle * 180.0 / M_PI);
            break;
        case 'z':
        default:
            rotationMatrix = createRotationZMatrix(angle * 180.0 / M_PI);
            break;
        }

        for (const auto &p : generatingPoints)
        {
            vertices.push_back(rotationMatrix.transform(p));
        }
    }

    std::vector<std::vector<int>> faces;
    int n = generatingPoints.size();

    for (int i = 0; i < steps; ++i)
    {
        int next_i = (i + 1) % steps;

        for (int j = 0; j < n - 1; ++j)
        {
            int p0 = i * n + j;
            int p1 = i * n + (j + 1);
            int p2 = next_i * n + (j + 1);
            int p3 = next_i * n + j;

            // Порядок должен быть ПРОТИВ часовой стрелки, если смотреть снаружи.

            // Треугольник 1: Низ-Текущий -> Низ-След -> Верх-Текущий
            faces.push_back({p0, p3, p1});

            // Треугольник 2: Верх-Текущий -> Низ-След -> Верх-След
            faces.push_back({p1, p3, p2});
        }
    }

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}

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
        // Создаем полигон без текстурных координат (по умолчанию)
        polygons.push_back(Polygon(polygonPoints));
    }
    return Polyhedron(polygons);
}

Polyhedron createTetrahedron()
{
    std::vector<Point3D> vertices = {
        Point3D(1, 1, 1),   // 0
        Point3D(-1, -1, 1), // 1
        Point3D(-1, 1, -1), // 2
        Point3D(1, -1, -1)  // 3
    };

    std::vector<std::vector<int>> faces = {
        {0, 2, 1},
        {0, 1, 3},
        {0, 3, 2},
        {1, 2, 3}};

    // UV-координаты для треугольной грани
    std::vector<TextureCoord> triUVs = {
        TextureCoord(0.0, 0.0),
        TextureCoord(1.0, 0.0),
        TextureCoord(0.5, 1.0)};

    std::vector<Polygon> polygons;
    for (const auto &face : faces)
    {
        std::vector<Point3D> polygonPoints;
        for (int idx : face)
        {
            polygonPoints.push_back(vertices[idx]);
        }
        // Передаем UV-координаты в конструктор Polygon
        polygons.push_back(Polygon(polygonPoints, triUVs));
    }

    return Polyhedron(polygons);
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
        {4, 5, 6, 7}, // Front
        {1, 0, 3, 2}, // Back
        {5, 1, 2, 6}, // Right
        {0, 4, 7, 3}, // Left
        {7, 6, 2, 3}, // Top
        {0, 1, 5, 4}  // Bottom
    };

    // UV-координаты для квадратной грани: (0,0), (1,0), (1,1), (0,1)
    std::vector<TextureCoord> quadUVs = {
        TextureCoord(0.0, 0.0),
        TextureCoord(1.0, 0.0),
        TextureCoord(1.0, 1.0),
        TextureCoord(0.0, 1.0)};

    std::vector<Polygon> polygons;
    for (const auto &face : faces)
    {
        std::vector<Point3D> polygonPoints;
        for (int idx : face)
        {
            polygonPoints.push_back(vertices[idx]);
        }
        polygons.push_back(Polygon(polygonPoints, quadUVs));
    }

    return Polyhedron(polygons);
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
        {0, 2, 4},
        {4, 2, 1},
        {1, 2, 5},
        {5, 2, 0},
        {0, 4, 3},
        {4, 1, 3},
        {1, 5, 3},
        {5, 0, 3}};

    // UV-координаты для треугольной грани
    std::vector<TextureCoord> triUVs = {
        TextureCoord(0.0, 0.0),
        TextureCoord(1.0, 0.0),
        TextureCoord(0.5, 1.0)};

    std::vector<Polygon> polygons;
    for (const auto &face : faces)
    {
        std::vector<Point3D> polygonPoints;
        for (int idx : face)
        {
            polygonPoints.push_back(vertices[idx]);
        }
        polygons.push_back(Polygon(polygonPoints, triUVs));
    }

    return Polyhedron(polygons);
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
        {0, 11, 5},
        {0, 5, 1},
        {0, 1, 7},
        {0, 7, 10},
        {0, 10, 11},
        {1, 5, 9},
        {5, 11, 4},
        {11, 10, 2},
        {10, 7, 6},
        {7, 1, 8},
        {3, 9, 4},
        {3, 4, 2},
        {3, 2, 6},
        {3, 6, 8},
        {3, 8, 9},
        {4, 9, 5},
        {2, 4, 11},
        {6, 2, 10},
        {8, 6, 7},
        {9, 8, 1}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}

Polyhedron createDodecahedron()
{
    double phi = (1.0 + sqrt(5.0)) / 2.0;
    double invPhi = 1.0 / phi;

    std::vector<Point3D> vertices = {
        Point3D(1, 1, 1), Point3D(1, 1, -1), Point3D(1, -1, 1), Point3D(1, -1, -1),
        Point3D(-1, 1, 1), Point3D(-1, 1, -1), Point3D(-1, -1, 1), Point3D(-1, -1, -1),
        Point3D(0, phi, invPhi), Point3D(0, phi, -invPhi), Point3D(0, -phi, invPhi), Point3D(0, -phi, -invPhi),
        Point3D(invPhi, 0, phi), Point3D(-invPhi, 0, phi), Point3D(invPhi, 0, -phi), Point3D(-invPhi, 0, -phi),
        Point3D(phi, invPhi, 0), Point3D(-phi, invPhi, 0), Point3D(phi, -invPhi, 0), Point3D(-phi, -invPhi, 0)};

    for (auto &v : vertices)
    {
        v = v * 0.4;
    }

    std::vector<std::vector<int>> faces = {
        {8, 4, 13, 12, 0},
        {12, 2, 18, 16, 0},
        {16, 1, 9, 8, 0},
        {16, 18, 3, 14, 1},
        {14, 15, 5, 9, 1},
        {12, 13, 6, 10, 2},
        {10, 11, 3, 18, 2},
        {11, 7, 15, 14, 3},
        {17, 19, 6, 13, 4},
        {8, 9, 5, 17, 4},
        {15, 7, 19, 17, 5},
        {19, 7, 11, 10, 6}};

    return createPolyhedronFromVerticesAndFaces(vertices, faces);
}