#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "object.h"
#include "geometry.h"

extern Polyhedron currentPolyhedron;
extern float rotationX;
extern float rotationY;
extern bool perspectiveProjection;
extern Matrix4x4 currentTransformation;
extern std::string currentMode;

void drawPolyhedron(sf::RenderWindow &window, const Polyhedron &poly,
                    const Matrix4x4 &modelViewProjectionMatrix);
void drawText(sf::RenderWindow &window, sf::Font &font, float x, float y, const std::string &text);
void display(sf::RenderWindow &window, sf::Font &font);

#endif
