#ifndef INPUT_H
#define INPUT_H

#include <SFML/Graphics.hpp>
#include "geometry.h"
#include "object.h"
#include "shapes.h"

extern Polyhedron currentPolyhedron;
extern float rotationX;
extern float rotationY;
extern bool perspectiveProjection;
extern Matrix4x4 currentTransformation;
extern unsigned int windowWidth;
extern unsigned int windowHeight;

void applyTransformation(const Matrix4x4 &transformation);
void applyTransformationAroundPolyhedronCenter(const Matrix4x4 &transformation);
void handleEvent(const sf::Event &event);

#endif
