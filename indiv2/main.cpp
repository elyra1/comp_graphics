#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <optional>

struct Vec3
{
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float d) const { return Vec3(x * d, y * d, z * d); }
    float dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const
    {
        float mag = std::sqrt(x * x + y * y + z * z);
        return (mag > 0) ? Vec3(x / mag, y / mag, z / mag) : Vec3(0, 0, 0);
    }
    Vec3 reflect(const Vec3 &n) const
    {
        return *this - n * 2.f * this->dot(n);
    }
};

struct Sphere
{
    Vec3 center;
    float radius;
    sf::Color color;
    bool intersect(const Vec3 &orig, const Vec3 &dir, float &t) const
    {
        Vec3 L = center - orig;
        float tca = L.dot(dir);
        if (tca < 0)
            return false;
        float d2 = L.dot(L) - tca * tca;
        if (d2 > radius * radius)
            return false;
        float thc = std::sqrt(radius * radius - d2);
        t = tca - thc;
        if (t < 0)
            t = tca + thc;
        return t > 0;
    }
};

struct Box
{
    Vec3 pos;
    Vec3 size;
    sf::Color color;
    bool intersect(const Vec3 &orig, const Vec3 &dir, float &tNear, Vec3 &normal) const
    {
        Vec3 minB = pos - (size * 0.5f);
        Vec3 maxB = pos + (size * 0.5f);
        float t1 = (minB.x - orig.x) / dir.x;
        float t2 = (maxB.x - orig.x) / dir.x;
        float t3 = (minB.y - orig.y) / dir.y;
        float t4 = (maxB.y - orig.y) / dir.y;
        float t5 = (minB.z - orig.z) / dir.z;
        float t6 = (maxB.z - orig.z) / dir.z;

        float tmin = std::max({std::min(t1, t2), std::min(t3, t4), std::min(t5, t6)});
        float tmax = std::min({std::max(t1, t2), std::max(t3, t4), std::max(t5, t6)});

        if (tmax < 0 || tmin > tmax)
            return false;
        tNear = tmin;
        Vec3 hitP = orig + dir * tNear;
        float b = 0.005f;

        if (std::abs(hitP.x - minB.x) < b)
            normal = Vec3(-1, 0, 0);
        else if (std::abs(hitP.x - maxB.x) < b)
            normal = Vec3(1, 0, 0);
        else if (std::abs(hitP.y - minB.y) < b)
            normal = Vec3(0, -1, 0);
        else if (std::abs(hitP.y - maxB.y) < b)
            normal = Vec3(0, 1, 0);
        else if (std::abs(hitP.z - minB.z) < b)
            normal = Vec3(0, 0, -1);
        else if (std::abs(hitP.z - maxB.z) < b)
            normal = Vec3(0, 0, 1);
        return true;
    }
};

struct Plane
{
    Vec3 point, normal;
    sf::Color color;
    bool isMirror = false;
};

sf::Color multiplyColor(sf::Color col, float intensity)
{
    return sf::Color(
        (std::uint8_t)(std::clamp(col.r * intensity, 0.0f, 255.0f)),
        (std::uint8_t)(std::clamp(col.g * intensity, 0.0f, 255.0f)),
        (std::uint8_t)(std::clamp(col.b * intensity, 0.0f, 255.0f)));
}

bool sceneIntersect(const Vec3 &orig, const Vec3 &dir,
                    const std::vector<Sphere> &spheres, const std::vector<Box> &boxes,
                    const std::vector<Plane> &walls,
                    float &tNear, Vec3 &hitNormal, sf::Color &hitColor, bool &isMirror)
{
    tNear = 1e20;
    bool hit = false;

    for (auto &p : walls)
    {
        float denom = p.normal.dot(dir);
        if (std::abs(denom) > 1e-6)
        {
            float t = (p.point - orig).dot(p.normal) / denom;
            if (t > 0 && t < tNear)
            {
                tNear = t;
                hitColor = p.color;
                hitNormal = p.normal;
                isMirror = p.isMirror;
                hit = true;
            }
        }
    }
    for (const auto &s : spheres)
    {
        float tS;
        if (s.intersect(orig, dir, tS) && tS < tNear)
        {
            tNear = tS;
            hitColor = s.color;
            hitNormal = ((orig + dir * tS) - s.center).normalize();
            isMirror = false;
            hit = true;
        }
    }
    for (const auto &b : boxes)
    {
        float tB;
        Vec3 nB;
        if (b.intersect(orig, dir, tB, nB) && tB < tNear)
        {
            tNear = tB;
            hitColor = b.color;
            hitNormal = nB;
            isMirror = false;
            hit = true;
        }
    }
    return hit;
}

sf::Color castRay(const Vec3 &orig, const Vec3 &dir,
                  const std::vector<Sphere> &spheres, const std::vector<Box> &boxes,
                  const std::vector<Plane> &walls, const std::vector<Vec3> &lights, int depth = 0)
{
    if (depth > 4)
        return sf::Color::Black;

    float t;
    Vec3 n;
    sf::Color color;
    bool mirror;
    if (!sceneIntersect(orig, dir, spheres, boxes, walls, t, n, color, mirror))
        return sf::Color(20, 20, 20);

    Vec3 hitPoint = orig + dir * t;

    if (mirror)
    {
        Vec3 reflectDir = dir.reflect(n).normalize();
        Vec3 reflectOrig = hitPoint + n * 0.001f;
        return castRay(reflectOrig, reflectDir, spheres, boxes, walls, lights, depth + 1);
    }

    float totalDiff = 0.0f;
    for (const auto &lp : lights)
    {
        Vec3 lightVec = lp - hitPoint;
        float dL = std::sqrt(lightVec.dot(lightVec));
        Vec3 toL = lightVec.normalize();
        Vec3 sO = hitPoint + n * 0.001f;

        bool blocked = false;
        float tmpT;
        Vec3 tmpN;
        sf::Color tmpC;
        bool tmpM;
        if (sceneIntersect(sO, toL, spheres, boxes, walls, tmpT, tmpN, tmpC, tmpM) && tmpT < dL)
            blocked = true;

        if (!blocked)
            totalDiff += std::max(0.0f, n.dot(toL)) * 0.6f;
    }
    return multiplyColor(color, 0.15f + totalDiff);
}

void renderScene(std::vector<std::uint8_t> &pixels, unsigned int w, unsigned int h,
                 const std::vector<Sphere> &spheres, const std::vector<Box> &boxes,
                 const std::vector<Plane> &walls, const std::vector<Vec3> &lights)
{
    for (unsigned int y = 0; y < h; y++)
    {
        for (unsigned int x = 0; x < w; x++)
        {
            float x_norm = (2.0f * (x + 0.5f) / (float)w - 1.0f);
            float y_norm = -(2.0f * (y + 0.5f) / (float)h - 1.0f);
            Vec3 rayDir = Vec3(x_norm, y_norm, -1.0f).normalize();
            Vec3 rayOrig(0, 0, 0);

            sf::Color finalColor = castRay(rayOrig, rayDir, spheres, boxes, walls, lights);

            size_t idx = (y * w + x) * 4;
            pixels[idx] = finalColor.r;
            pixels[idx + 1] = finalColor.g;
            pixels[idx + 2] = finalColor.b;
            pixels[idx + 3] = 255;
        }
    }
}

int main()
{
    const unsigned int width = 800;
    const unsigned int height = 800;
    sf::RenderWindow window(sf::VideoMode({width, height}), "Raytracing: All Controls & Mirrors");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    std::vector<std::uint8_t> pixels(width * height * 4);
    sf::Texture texture;
    texture.resize({width, height});
    sf::Sprite sprite(texture);

    std::vector<Sphere> spheres = {
        {Vec3(1.0f, -2.5f, -6.0f), 1.0f, sf::Color::Yellow},
        {Vec3(-1.5f, -2.5f, -4.5f), 0.7f, sf::Color::Green}};

    std::vector<Box> boxes = {
        {Vec3(-1.0f, -2.5f, -6.5f), Vec3(1.2f, 1.2f, 1.2f), sf::Color::Cyan},
        {Vec3(1.8f, -3.0f, -4.0f), Vec3(0.8f, 0.8f, 0.8f), sf::Color::Magenta}};

    std::vector<Vec3> lights = {Vec3(-2.0f, 3.0f, -4.0f), Vec3(2.0f, 3.0f, -6.0f)};

    std::vector<Plane> walls = {
        {Vec3(0, -4, 0), Vec3(0, 1, 0), sf::Color::White, false},     // Пол
        {Vec3(0, 4, 0), Vec3(0, -1, 0), sf::Color::White, false},     // Потолок
        {Vec3(-4, 0, 0), Vec3(1, 0, 0), sf::Color(200, 0, 0), false}, // Левая
        {Vec3(4, 0, 0), Vec3(-1, 0, 0), sf::Color(0, 0, 200), false}, // Правая
        {Vec3(0, 0, -10), Vec3(0, 0, 1), sf::Color::White, false},    // Задняя
    };

    renderScene(pixels, width, height, spheres, boxes, walls, lights);
    texture.update(pixels.data());

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Scene Control");

        if (ImGui::CollapsingHeader("Mirror Walls"))
        {
            const char *wallNames[] = {"Floor", "Ceiling", "Left Wall", "Right Wall", "Back Wall"};
            for (int i = 0; i < walls.size(); i++)
            {
                ImGui::Checkbox(wallNames[i], &walls[i].isMirror);
            }
        }

        if (ImGui::CollapsingHeader("Spheres"))
        {
            for (int i = 0; i < spheres.size(); i++)
            {
                ImGui::PushID(i);
                ImGui::Text("Sphere %d", i + 1);
                ImGui::SliderFloat("X", &spheres[i].center.x, -3.5f, 3.5f);
                ImGui::SliderFloat("Y", &spheres[i].center.y, -3.5f, 3.5f);
                ImGui::SliderFloat("Z", &spheres[i].center.z, -9.5f, -2.0f);
                ImGui::PopID();
            }
        }

        if (ImGui::CollapsingHeader("Cubes"))
        {
            for (int i = 0; i < boxes.size(); i++)
            {
                ImGui::PushID(i + 100);
                ImGui::Text("Cube %d", i + 1);
                ImGui::SliderFloat("X", &boxes[i].pos.x, -3.5f, 3.5f);
                ImGui::SliderFloat("Y", &boxes[i].pos.y, -3.5f, 3.5f);
                ImGui::SliderFloat("Z", &boxes[i].pos.z, -9.5f, -2.0f);
                ImGui::PopID();
            }
        }

        if (ImGui::CollapsingHeader("Lights"))
        {
            for (int i = 0; i < lights.size(); i++)
            {
                ImGui::PushID(i + 200);
                ImGui::Text("Light %d", i + 1);
                ImGui::SliderFloat("Light X", &lights[i].x, -3.9f, 3.9f);
                ImGui::SliderFloat("Light Y", &lights[i].y, -3.9f, 3.9f);
                ImGui::SliderFloat("Light Z", &lights[i].z, -9.9f, -1.0f);
                ImGui::PopID();
            }
        }

        if (ImGui::Button("Apply Changes", ImVec2(-1, 40)))
        {
            renderScene(pixels, width, height, spheres, boxes, walls, lights);
            texture.update(pixels.data());
        }
        ImGui::End();

        window.clear();
        window.draw(sprite);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}