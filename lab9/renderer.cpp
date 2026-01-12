#include "renderer.h"
#include "input.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>
#include <cstdint>
#include <array>

// Параметры источника света (позиция и цвет)
Light sceneLight = {{0.0, 5.0, 1.0}, {1.0, 1.0, 1.0}};

// Текущий режим закрашивания (Gouraud, Toon)
ShadingMode currentShadingMode = ShadingMode::GOURAUD;

bool enableBackfaceCulling = false;
bool enableZBuffer = false;
bool enableLighting = true;

// Texture-related globals
bool enableTexturing = false;
sf::Texture texture;
bool textureLoaded = false;

extern unsigned int windowWidth;
extern unsigned int windowHeight;

static std::vector<float> zBuffer;
static std::vector<std::uint8_t> frameBuffer;

static sf::Texture renderTexture;
static sf::Sprite renderSprite(renderTexture);

// Структура вершины для этапа растеризации
struct ScreenVertex
{
    sf::Vector2f pos; // Координаты на экране (в пикселях)
    float z;          // Глубина (z-координата после проекции)
    Point3D worldPos; // Координаты в мировом пространстве (нужны для Phong/Toon шейдинга)
    Vector3D normal;  // Нормаль в этой вершине (нужна для интерполяции в Phong/Toon)
    float intensity;  // Рассчитанная интенсивность света (используется только для Gouraud)
    float u, v;       // Текстурные координаты (интерполируемые)
    float invW;
};

// Преобразование нормализованных координат в экранные координаты
static sf::Vector2f project(const Point3D &p)
{
    return sf::Vector2f(
        (p.x + 1.0f) * windowWidth / 2.0f,
        (-p.y + 1.0f) * windowHeight / 2.0f);
}

// Вычисляет барицентрические координаты.
// Определяет положение точки относительно прямой, проходящей через a и b.
inline static float edgeFunction(const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

// Расчет интенсивности для Toon Shading.
// Разбивает плавный градиент освещения на несколько ступеней.
static float calculateToonIntensity(float diffuseDot)
{
    // Смещение для настройки порогов
    float diff = 0.2f + std::max(0.0f, diffuseDot);

    if (diff < 0.4f)
        return 0.3f; // Область тени
    else if (diff < 0.7f)
        return 1.0f; // Основной цвет
    else
        return 1.3f; // Блик (Highlight)
}

// Проверка видимости грани (Backface Culling).
// Вычисляет скалярное произведение нормали грани и вектора взгляда.
static bool isFaceVisible(const std::vector<Point3D> &points, const Matrix4x4 &modelMatrix, const Vector3D &normal)
{
    if (points.empty())
        return false;

    // Находим центр грани
    Point3D faceCenter;
    for (const auto &p : points)
        faceCenter = faceCenter + p;
    faceCenter = faceCenter * (1.0 / points.size());

    // Трансформируем центр и нормаль в мировое пространство
    Point3D transformedFaceCenter = modelMatrix.transform(faceCenter);
    Vector3D transformedNormal = modelMatrix.transform(normal);

    // Вектор от поверхности к камере
    Vector3D viewVector = camera.position - transformedFaceCenter;

    return transformedNormal.dot(viewVector) >= 0;
}

// Обновление размеров буферов при изменении размера окна
static void updateRenderBuffers()
{
    size_t totalPixels = windowWidth * windowHeight;
    if (zBuffer.size() != totalPixels)
    {
        zBuffer.resize(totalPixels);
        frameBuffer.resize(totalPixels * 4); // R, G, B, A
        renderTexture.resize({windowWidth, windowHeight});
        renderSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(windowWidth), static_cast<int>(windowHeight)}));
    }

    // Очистка буферов перед новым кадром
    std::fill(frameBuffer.begin(), frameBuffer.end(), 0);
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
}

// Отрисовка координатных осей
static void drawAxes(sf::RenderWindow &window, const Matrix4x4 &viewProjection)
{
    Point3D origin(0, 0, 0);
    Point3D xAxis(3, 0, 0);
    Point3D yAxis(0, 3, 0);
    Point3D zAxis(0, 0, 3);

    sf::VertexArray axisLines(sf::PrimitiveType::Lines, 6);

    // X
    axisLines[0].position = project(viewProjection.transform(origin));
    axisLines[0].color = sf::Color::Red;
    axisLines[1].position = project(viewProjection.transform(xAxis));
    axisLines[1].color = sf::Color::Red;

    // Y
    axisLines[2].position = project(viewProjection.transform(origin));
    axisLines[2].color = sf::Color::Green;
    axisLines[3].position = project(viewProjection.transform(yAxis));
    axisLines[3].color = sf::Color::Green;

    // Z
    axisLines[4].position = project(viewProjection.transform(origin));
    axisLines[4].color = sf::Color::Blue;
    axisLines[5].position = project(viewProjection.transform(zAxis));
    axisLines[5].color = sf::Color::Blue;

    window.draw(axisLines);
}

void drawText(sf::RenderWindow &window, sf::Font &font, float x, float y, const std::string &text)
{
    sf::Text txt(font, text, 14);
    txt.setPosition({x, y});
    txt.setFillColor(sf::Color::Black);
    window.draw(txt);
}

// Вычисление цвета конкретного пикселя.
static sf::Color computePixelColor(
    const Vector3D &baseColor,
    float w0, float w1, float w2, // Барицентрические веса
    const ScreenVertex &v0, const ScreenVertex &v1, const ScreenVertex &v2,
    float u, float v,         // Интерполированные текстурные координаты
    const sf::Image *texture) // Текстура объекта (изображение в памяти)
{
    // Определяем базовый цвет (из текстуры или цвета объекта)
    Vector3D base = baseColor;

    if (enableTexturing && texture && texture->getSize().x > 0)
    {
        unsigned int texWidth = texture->getSize().x;
        unsigned int texHeight = texture->getSize().y;

        // Преобразование нормализованных координат в пиксельные
        // Ограничиваем координаты диапазоном [0, 1] (clamp)
        float u_clamped = std::max(0.0f, std::min(1.0f, u));
        float v_clamped = std::max(0.0f, std::min(1.0f, v));

        unsigned int texX = (u_clamped * (texWidth - 1));
        unsigned int texY = (v_clamped * (texHeight - 1));

        sf::Color texColor = texture->getPixel(sf::Vector2u(texX, texY));
        base = Vector3D(texColor.r / 255.0, texColor.g / 255.0, texColor.b / 255.0);
    }

    if (!enableLighting)
    {
        sf::Color color;
        color.r = std::min(static_cast<float>(base.x) * 255.f, 255.f);
        color.g = std::min(static_cast<float>(base.y) * 255.f, 255.f);
        color.b = std::min(static_cast<float>(base.z) * 255.f, 255.f);
        return color;
    }

    float r_base = base.x * sceneLight.color.x;
    float g_base = base.y * sceneLight.color.y;
    float b_base = base.z * sceneLight.color.z;

    float finalFactor = 0.0f;

    if (currentShadingMode == ShadingMode::GOURAUD)
    {
        // Шейдинг Гуро: Интерполяция уже рассчитанных в вершинах интенсивностей
        finalFactor = w0 * v0.intensity + w1 * v1.intensity + w2 * v2.intensity;
    }
    else
    {
        // Шейдинг Фонга, Toon
        // 1. Интерполируем нормаль для текущего пикселя
        Vector3D pixelNormal = (v0.normal * w0 + v1.normal * w1 + v2.normal * w2).normalized();

        // 2. Интерполируем позицию пикселя в мировом пространстве
        Point3D pixelWorldPos = v0.worldPos * w0 + v1.worldPos * w1 + v2.worldPos * w2;

        // 3. Рассчитываем вектор света для этого пикселя
        Vector3D lightDir = (sceneLight.position - pixelWorldPos).normalized();

        // 4. Скалярное произведение
        float dotProd = pixelNormal.dot(lightDir);

        if (currentShadingMode == ShadingMode::TOON)
        {
            // Ступенчатое освещение
            finalFactor = calculateToonIntensity(dotProd);
        }
        else
        {
            // Обычный диффузный свет
            finalFactor = std::max(0.0f, dotProd);
        }
    }

    // Применяем рассчитанную интенсивность к базовому цвету
    sf::Color color;
    color.r = std::min(static_cast<float>(r_base * finalFactor) * 255.f, 255.f);
    color.g = std::min(static_cast<float>(g_base * finalFactor) * 255.f, 255.f);
    color.b = std::min(static_cast<float>(b_base * finalFactor) * 255.f, 255.f);

    return color;
}

// Растеризация треугольника: заполнение пикселей внутри треугольника
static void rasterizeTriangle(
    const std::array<ScreenVertex, 3> &verts,
    const Vector3D &objColor,
    const sf::Image *texture)
{
    const auto &v0 = verts[0];
    const auto &v1 = verts[1];
    const auto &v2 = verts[2];

    float minX = std::min({v0.pos.x, v1.pos.x, v2.pos.x});
    float maxX = std::max({v0.pos.x, v1.pos.x, v2.pos.x});
    float minY = std::min({v0.pos.y, v1.pos.y, v2.pos.y});
    float maxY = std::max({v0.pos.y, v1.pos.y, v2.pos.y});

    // Обрезаем по границам экрана
    int x_start = std::max(0, static_cast<int>(std::floor(minX)));
    int x_end = std::min(static_cast<int>(windowWidth) - 1, static_cast<int>(std::ceil(maxX)));
    int y_start = std::max(0, static_cast<int>(std::floor(minY)));
    int y_end = std::min(static_cast<int>(windowHeight) - 1, static_cast<int>(std::ceil(maxY)));

    float area = edgeFunction(v0.pos, v1.pos, v2.pos);

    // Игнорируем вырожденные треугольники
    if (std::abs(area) < 1e-5)
        return;

    float invArea = 1.0f / area;

    // Проход по всем пикселям в bounding box
    for (int y = y_start; y <= y_end; ++y)
    {
        for (int x = x_start; x <= x_end; ++x)
        {
            sf::Vector2f p(x + 0.5f, y + 0.5f); // Центр пикселя

            // Вычисление барицентрических координат (весов)
            float w0 = edgeFunction(v1.pos, v2.pos, p);
            float w1 = edgeFunction(v2.pos, v0.pos, p);
            float w2 = edgeFunction(v0.pos, v1.pos, p);

            // Проверяем, находится ли пиксель внутри треугольника
            bool is_inside = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);

            if (is_inside)
            {
                // Нормализация весов (сумма весов равна 1)
                w0 *= invArea;
                w1 *= invArea;
                w2 *= invArea;

                float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                int idx = y * windowWidth + x;

                if (z < zBuffer[idx])
                {
                    zBuffer[idx] = z;

                    // 1. Интерполируем 1/W
                    float interpolatedInvW = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;

                    // 2. Интерполируем U/W и V/W
                    float interpolatedU = w0 * v0.u + w1 * v1.u + w2 * v2.u;
                    float interpolatedV = w0 * v0.v + w1 * v1.v + w2 * v2.v;

                    float finalU = interpolatedU / interpolatedInvW;
                    float finalV = interpolatedV / interpolatedInvW;

                    // Вычисляем цвет пикселя
                    sf::Color finalColor = computePixelColor(objColor, w0, w1, w2, v0, v1, v2, finalU, finalV, texture);

                    // Запись цвета во фреймбуфер
                    int pixelIdx = idx * 4;
                    frameBuffer[pixelIdx] = finalColor.r;
                    frameBuffer[pixelIdx + 1] = finalColor.g;
                    frameBuffer[pixelIdx + 2] = finalColor.b;
                    frameBuffer[pixelIdx + 3] = 255;
                }
            }
        }
    }
}

// Отрисовка в обычном режиме, когда выключен Z-buffer
void drawPolyhedron(sf::RenderWindow &window, const Polyhedron &poly,
                    const Matrix4x4 &modelMatrix,
                    const Matrix4x4 &modelViewProjectionMatrix)
{
    for (const auto &polygon : poly.polygons)
    {
        if (polygon.points.empty())
            continue;

        // Отсечение нелицевых граней
        if (enableBackfaceCulling)
        {
            if (!isFaceVisible(polygon.points, modelMatrix, polygon.normal))
                continue;
        }

        std::vector<sf::Vector2f> projectedPoints;
        projectedPoints.reserve(polygon.points.size());

        for (const auto &point : polygon.points)
        {
            Point3D transformedPoint = modelViewProjectionMatrix.transform(point);
            projectedPoints.push_back(project(transformedPoint));
        }

        // Рисуем линии между точками
        sf::VertexArray lines(sf::PrimitiveType::LineStrip, projectedPoints.size() + 1);
        for (size_t j = 0; j < projectedPoints.size(); ++j)
        {
            lines[j].position = projectedPoints[j];
            lines[j].color = sf::Color::Black;
        }
        // Замыкаем контур
        lines[projectedPoints.size()].position = projectedPoints[0];
        lines[projectedPoints.size()].color = sf::Color::Black;
        window.draw(lines);

        // Рисуем вершины в виде маленьких кругов
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

// Основная функция отрисовки сцены
void display(sf::RenderWindow &window, sf::Font &font, const std::vector<SceneObject> &objects)
{
    // Обновляем соотношение сторон камеры
    camera.aspectRatio = (float)windowWidth / (float)windowHeight;

    // Получаем матрицы вида и проекции
    Matrix4x4 viewMatrix = camera.getViewMatrix();
    Matrix4x4 projectionMatrix = camera.getProjectionMatrix(perspectiveProjection);
    Matrix4x4 viewProjection = projectionMatrix * viewMatrix;

    // Рисуем оси координат
    drawAxes(window, viewProjection);

    if (enableZBuffer)
    {
        // Режим растеризации
        updateRenderBuffers();

        for (const auto &object : objects)
        {
            Matrix4x4 modelMatrix = object.currentTransformation;
            Matrix4x4 mvp = viewProjection * modelMatrix;

            // Подготовка текстуры для программного рендеринга.
            sf::Image objectTextureImage;
            const sf::Image *pTextureImage = nullptr;

            if (object.texture && object.texture->getSize().x > 0)
            {
                objectTextureImage = object.texture->copyToImage();
                pTextureImage = &objectTextureImage;
            }

            for (const auto &poly : object.polyhedron.polygons)
            {
                if (poly.points.size() < 3)
                    continue;

                // Отсечение нелицевых граней
                if (enableBackfaceCulling)
                {
                    if (!isFaceVisible(poly.points, modelMatrix, poly.normal))
                        continue;
                }

                // Подготовка вершин многоугольника к растеризации
                std::vector<ScreenVertex> polyScreenVerts;
                polyScreenVerts.reserve(poly.points.size());

                Matrix4x4 modelView = viewMatrix * modelMatrix;

                for (size_t k = 0; k < poly.points.size(); ++k)
                {
                    // Перевод в экранные координаты
                    Point3D ndc = mvp.transform(poly.points[k]);
                    float sx = (ndc.x + 1.0f) * windowWidth * 0.5f;
                    float sy = (-ndc.y + 1.0f) * windowHeight * 0.5f;

                    ScreenVertex sv;
                    sv.pos = {sx, sy};
                    sv.z = ndc.z;
                    sv.worldPos = modelMatrix.transform(poly.points[k]); // Мировая позиция

                    // Расчет нормали для вершины
                    if (poly.vertexNormals.size() == poly.points.size())
                    {
                        sv.normal = modelMatrix.transform(poly.vertexNormals[k]).normalized();
                    }
                    else
                    {
                        sv.normal = modelMatrix.transform(poly.normal).normalized();
                    }

                    Point3D viewSpacePos = modelView.transform(poly.points[k]);
                    // Обычно камера смотрит вдоль -Z, поэтому глубина это -Z.
                    // Либо можно взять просто модуль, чтобы избежать отрицательных значений.
                    float depth = std::abs(viewSpacePos.z);
                    if (depth < 1e-5f)
                        depth = 1e-5f; // Защита от деления на 0

                    sv.invW = 1.0f / depth; // Сохраняем 1/W

                    // Сохраняем U/W и V/W вместо чистых U и V
                    if (k < poly.textureCoords.size())
                    {
                        sv.u = poly.textureCoords[k].u * sv.invW;
                        sv.v = poly.textureCoords[k].v * sv.invW;
                    }
                    else
                    {
                        sv.u = 0.0f;
                        sv.v = 0.0f;
                    }
                    // Для шейдинга Гуро освещение считается В ВЕРШИНАХ
                    sv.intensity = 0.0f;
                    if (currentShadingMode == ShadingMode::GOURAUD)
                    {
                        auto dot_value = (sv.normal.dot((sceneLight.position - sv.worldPos).normalized()));
                        sv.intensity = std::max(0.0f, static_cast<float>(dot_value));
                    }

                    polyScreenVerts.push_back(sv);
                }

                // Триангуляция полигона и растеризация
                // Полигон разбивается на треугольники веером (fan triangulation) от нулевой вершины
                for (size_t i = 1; i + 1 < polyScreenVerts.size(); ++i)
                {
                    std::array<ScreenVertex, 3> triangle = {
                        polyScreenVerts[0],
                        polyScreenVerts[i],
                        polyScreenVerts[i + 1]};

                    // Передаем указатель на подготовленное изображение
                    rasterizeTriangle(triangle, object.color, pTextureImage);
                }
            }
        }

        // Загружаем пиксели в текстуру и рисуем
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