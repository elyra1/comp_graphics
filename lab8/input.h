#ifndef INPUT_H
#define INPUT_H

#include <SFML/Graphics.hpp>
#include "geometry.h"
#include "object.h"
#include "shapes.h"
#include "camera.h"

extern Camera camera;
extern SceneObject currentSceneObject;
extern bool perspectiveProjection;
extern unsigned int windowWidth;
extern unsigned int windowHeight;
extern std::string currentMode;

void applyTransformation(const Matrix4x4 &transformation);
void applyTransformationAroundPolyhedronCenter(const Matrix4x4 &transformation);

#endif
