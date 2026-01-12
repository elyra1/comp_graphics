#include "renderer.h"
#include "input.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>
#include <cstdint>

bool enableBackfaceCulling = false;
bool enableZBuffer = false;

extern unsigned int windowWidth;
extern unsigned int windowHeight;

static std::vector<float> zBuffer;
static std::vector<std::uint8_t> frameBuffer;

static sf::Texture renderTexture;
static sf::Sprite renderSprite(renderTexture);

// Функция для проекции 3D точки в 2D пространство окна
static sf::Vector2f project(const Point3D &p)
{
    return sf::Vector2f(
        (static_cast<float>(p.x) + 1.0f) * windowWidth / 2.0f,
        (static_cast<float>(-p.y) + 1.0f) * windowHeight / 2.0f);
}

// Барицентрические координаты
inline static float edgeFunction(const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void drawPolyhedron(sf::RenderWindow &window, const Polyhedron &poly,
                    const Matrix4x4 &modelMatrix,
                    const Matrix4x4 &modelViewProjectionMatrix)
{
    for (size_t i = 0; i < poly.polygons.size(); ++i)
    {
        auto polygon = poly.polygons[i];
        if (polygon.points.empty())
            continue;

        if (enableBackfaceCulling)
        {
            Point3D faceCenter;
            for (const auto &p : polygon.points)
                faceCenter = faceCenter + p;
            if (!polygon.points.empty())
                faceCenter = faceCenter * (1.0 / polygon.points.size());

            Point3D transformedFaceCenter = modelMatrix.transform(faceCenter);
            Vector3D transformedNormal = modelMatrix.transform(polygon.normal);
            Vector3D viewVector = camera.position - transformedFaceCenter;

            if (transformedNormal.dot(viewVector) < 0)
                continue;
        }

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

void display(sf::RenderWindow &window, sf::Font &font, const std::vector<SceneObject> &objects)
{
    camera.aspectRatio = (float)windowWidth / (float)windowHeight;

    Matrix4x4 viewMatrix = camera.getViewMatrix();
    Matrix4x4 projectionMatrix = camera.getProjectionMatrix(perspectiveProjection);
    Matrix4x4 viewProjection = projectionMatrix * viewMatrix;

    // Рисуем оси
    Point3D origin(0, 0, 0);
    Point3D xAxis(3, 0, 0);
    Point3D yAxis(0, 3, 0);
    Point3D zAxis(0, 0, 3);

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

    if (enableZBuffer)
    {
        // Проверка размера буферов
        size_t totalPixels = windowWidth * windowHeight;

        // Проверяем, изменился ли размер окна
        if (zBuffer.size() != totalPixels)
        {
            zBuffer.resize(totalPixels);
            frameBuffer.resize(totalPixels * 4);
            renderTexture.resize({windowWidth, windowHeight});

            // Обновляем область отображения спрайта
            renderSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(windowWidth), static_cast<int>(windowHeight)}));
        }

        // Очистка буфера кадра
        std::fill(frameBuffer.begin(), frameBuffer.end(), 0);

        // Очистка z-буфера (максимальная глубина)
        std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());

        // Растеризация полигонов
        for (const auto &object : objects)
        {
            Matrix4x4 mvp = viewProjection * object.currentTransformation;
            size_t polyIndex = 0;

            for (const auto &poly : object.polyhedron.polygons)
            {
                polyIndex++;
                if (poly.points.empty())
                    continue;

                // Удаление нелицевых граней
                if (enableBackfaceCulling)
                {
                    Point3D faceCenter;
                    for (const auto &p : poly.points)
                        faceCenter = faceCenter + p;
                    faceCenter = faceCenter * (1.0 / poly.points.size());

                    Point3D transformedFaceCenter = object.currentTransformation.transform(faceCenter);
                    Vector3D transformedNormal = object.currentTransformation.transform(poly.normal);
                    Vector3D viewVector = camera.position - transformedFaceCenter;

                    if (transformedNormal.dot(viewVector) < 0)
                        continue;
                }

                struct Vertex
                {
                    sf::Vector2f pos;
                    float z;
                };
                std::vector<Vertex> screenVerts;
                screenVerts.reserve(poly.points.size());

                for (const auto &p : poly.points)
                {
                    Point3D ndc = mvp.transform(p);

                    float sx = (static_cast<float>(ndc.x) + 1.0f) * windowWidth * 0.5f;
                    float sy = (static_cast<float>(-ndc.y) + 1.0f) * windowHeight * 0.5f;

                    screenVerts.push_back({{sx, sy}, static_cast<float>(ndc.z)});
                }

                sf::Color polyColor((polyIndex * 30) % 255, (polyIndex * 50) % 255, (polyIndex * 70) % 255, 255);

                // Триангуляция (triangle fan)
                for (size_t i = 1; i + 1 < screenVerts.size(); ++i)
                {
                    const Vertex &v0 = screenVerts[0];
                    const Vertex &v1 = screenVerts[i];
                    const Vertex &v2 = screenVerts[i + 1];

                    // Bounding Box
                    float minX = std::min({v0.pos.x, v1.pos.x, v2.pos.x});
                    float maxX = std::max({v0.pos.x, v1.pos.x, v2.pos.x});
                    float minY = std::min({v0.pos.y, v1.pos.y, v2.pos.y});
                    float maxY = std::max({v0.pos.y, v1.pos.y, v2.pos.y});

                    int x0 = std::max(0, static_cast<int>(std::floor(minX)));
                    int x1 = std::min(static_cast<int>(windowWidth) - 1, static_cast<int>(std::ceil(maxX)));
                    int y0 = std::max(0, static_cast<int>(std::floor(minY)));
                    int y1 = std::min(static_cast<int>(windowHeight) - 1, static_cast<int>(std::ceil(maxY)));

                    float area = edgeFunction(v0.pos, v1.pos, v2.pos);
                    if (std::abs(area) < 1e-5)
                        continue;

                    // Попиксельный обход
                    for (int y = y0; y <= y1; ++y)
                    {
                        for (int x = x0; x <= x1; ++x)
                        {
                            sf::Vector2f p(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);

                            float w0 = edgeFunction(v1.pos, v2.pos, p);
                            float w1 = edgeFunction(v2.pos, v0.pos, p);
                            float w2 = edgeFunction(v0.pos, v1.pos, p);

                            // Если точка внутри треугольника
                            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                            {
                                w0 /= area;
                                w1 /= area;
                                w2 /= area;

                                // Вычислить Z
                                float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

                                int idx = y * windowWidth + x;

                                // Сравнить Z с буфером
                                if (z < zBuffer[idx])
                                {
                                    // Записать в буфер
                                    zBuffer[idx] = z;

                                    int pixelIdx = idx * 4;
                                    frameBuffer[pixelIdx] = polyColor.r;
                                    frameBuffer[pixelIdx + 1] = polyColor.g;
                                    frameBuffer[pixelIdx + 2] = polyColor.b;
                                    frameBuffer[pixelIdx + 3] = 255;
                                }
                            }
                        }
                    }
                }
            }
        }

        renderTexture.update(frameBuffer.data());
        window.draw(renderSprite);
    }
    else
    {
        for (const auto &object : objects)
        {
            Matrix4x4 modelMatrix = object.currentTransformation;
            Matrix4x4 mvp = viewProjection * modelMatrix;
            drawPolyhedron(window, object.polyhedron, modelMatrix, mvp);
        }
    }
}