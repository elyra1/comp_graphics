#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "object.h"
#include "geometry.h"

struct Light
{
    Point3D position;
    Point3D color;
};

enum class ShadingMode
{
    GOURAUD,
    TOON
};

extern Light sceneLight;
extern ShadingMode currentShadingMode;
extern SceneObject currentSceneObject;
extern float rotationX;
extern float rotationY;
extern bool perspectiveProjection;
extern bool enableBackfaceCulling;
extern bool enableZBuffer;
extern bool enableLighting;
extern bool enableTexturing;
extern sf::Texture texture;
extern bool textureLoaded;
extern std::string currentMode;

void drawPolyhedron(sf::RenderWindow &window, const Polyhedron &poly,
                    const Matrix4x4 &modelMatrix,
                    const Matrix4x4 &modelViewProjectionMatrix);
void drawText(sf::RenderWindow &window, sf::Font &font, float x, float y, const std::string &text);
void display(sf::RenderWindow &window, sf::Font &font, const std::vector<SceneObject> &objects);

#endif
