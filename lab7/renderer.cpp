#include "renderer.h"
#include <iostream>
#include <algorithm>
#include <vector>

extern unsigned int windowWidth;
extern unsigned int windowHeight;

// Функция для проекции 3D точки в 2D пространство окна
sf::Vector2f project(const Point3D &p)
{
    // Преобразование из нормализованных координат (-1, 1) в экранные координаты
    return sf::Vector2f(
        (static_cast<float>(p.x) + 1.0f) * windowWidth / 2.0f,
        (static_cast<float>(-p.y) + 1.0f) * windowHeight / 2.0f);
}

void drawPolyhedron(sf::RenderWindow &window, const Polyhedron &poly, const Matrix4x4 &modelViewProjectionMatrix)
{
    for (size_t i = 0; i < poly.polygons.size(); ++i)
    {
        const auto &polygon = poly.polygons[i];
        if (polygon.points.empty())
            continue;

        std::vector<sf::Vector2f> projectedPoints;
        for (const auto &point : polygon.points)
        {
            Point3D transformedPoint = modelViewProjectionMatrix.transform(point);
            projectedPoints.push_back(project(transformedPoint));
        }

        sf::VertexArray lines(sf::PrimitiveType::LineStrip, projectedPoints.size() + 1);
        for (size_t j = 0; j < projectedPoints.size(); ++j)
        {
            lines[j].position = projectedPoints[j];
            lines[j].color = sf::Color::Black;
        }
        lines[projectedPoints.size()].position = projectedPoints[0];
        lines[projectedPoints.size()].color = sf::Color::Black;
        window.draw(lines);

        // Отрисовка вершин
        for (const auto &pos : projectedPoints)
        {
            sf::CircleShape circle(3.f);
            circle.setOrigin({3.f, 3.f});
            circle.setPosition(pos);
            circle.setFillColor(sf::Color::Black);
            window.draw(circle);
        }
    }
}

void drawText(sf::RenderWindow &window, sf::Font &font, float x, float y, const std::string &text)
{
    sf::Text txt(font, text, 14);
    txt.setPosition({x, y});
    txt.setFillColor(sf::Color::Black);
    window.draw(txt);
}

void display(sf::RenderWindow &window, sf::Font &font)
{
    // Создаем матрицы вида и проекции
    Matrix4x4 modelMatrix = currentTransformation;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;

    viewMatrix.identity();
    viewMatrix = createRotationXMatrix(30) * viewMatrix;
    viewMatrix = createTranslationMatrix(0, 0, -6) * viewMatrix;

    if (perspectiveProjection)
    {
        projectionMatrix = createPerspectiveMatrix(45.0, (double)windowWidth / windowHeight, 1.0, 100.0);
    }
    else
    {
        projectionMatrix = createAxonometricMatrix(-2.5, 2.5, -2.5, 2.5, -100.0, 100.0);
    }

    // Добавляем вращение камеры
    Matrix4x4 cameraRotation = createRotationYMatrix(rotationY) * createRotationXMatrix(rotationX);
    viewMatrix = viewMatrix * cameraRotation;

    Matrix4x4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    // Рисуем оси координат
    Point3D origin(0, 0, 0);
    Point3D xAxis(3, 0, 0);
    Point3D yAxis(0, 3, 0);
    Point3D zAxis(0, 0, 3);
    Matrix4x4 viewProjection = projectionMatrix * viewMatrix;

    sf::VertexArray axisLines(sf::PrimitiveType::Lines, 6);
    axisLines[0].position = project(viewProjection.transform(origin));
    axisLines[0].color = sf::Color::Red;
    axisLines[1].position = project(viewProjection.transform(xAxis));
    axisLines[1].color = sf::Color::Red;
    axisLines[2].position = project(viewProjection.transform(origin));
    axisLines[2].color = sf::Color::Green;
    axisLines[3].position = project(viewProjection.transform(yAxis));
    axisLines[3].color = sf::Color::Green;
    axisLines[4].position = project(viewProjection.transform(origin));
    axisLines[4].color = sf::Color::Blue;
    axisLines[5].position = project(viewProjection.transform(zAxis));
    axisLines[5].color = sf::Color::Blue;
    window.draw(axisLines);

    // Рисуем многогранник
    drawPolyhedron(window, currentPolyhedron, mvp);
}