#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "geometry.h"        // Включает Point3D, Vector3D, Matrix4x4, TextureCoord
#include <SFML/Graphics.hpp> // Для sf::Texture

class Polygon
{
public:
    std::vector<Point3D> points;
    Vector3D normal;
    std::vector<Vector3D> vertexNormals;     // Нормали для каждой вершины
    std::vector<TextureCoord> textureCoords; // Новый: текстурные координаты для каждой вершины

    // Конструктор, модифицированный для принятия текстурных координат
    Polygon(const std::vector<Point3D> &points = {}, const std::vector<TextureCoord> &textureCoords = {});
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
    void calculateVertexNormals(); // Метод для расчета нормалей вершин
};

class SceneObject
{
public:
    Polyhedron polyhedron;
    Matrix4x4 currentTransformation;
    Vector3D color;
    sf::Texture *texture = nullptr;
    bool hasTexture = false;

    SceneObject(const Polyhedron &p = Polyhedron(),
                const Matrix4x4 &t = Matrix4x4(),
                const Vector3D &c = Vector3D(1.0, 1.0, 1.0),
                sf::Texture *tex = nullptr)
        : polyhedron(p), currentTransformation(t), color(c), texture(tex), hasTexture(tex != nullptr) {}
};

#endif