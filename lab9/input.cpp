#include "input.h"
#include "renderer.h"
#include <iostream>
#include <variant>
#include "geometry.h"
#include "object.h"
#include <vector>

extern std::vector<SceneObject> sceneObjects;
extern int currentIndex;

void applyTransformation(const Matrix4x4 &transformationMatrix)
{
    sceneObjects[currentIndex].currentTransformation = transformationMatrix * sceneObjects[currentIndex].currentTransformation;
}

void applyTransformationAroundPolyhedronCenter(const Matrix4x4 &transformationMatrix)
{
    Point3D center = sceneObjects[currentIndex].polyhedron.getCenter();
    Matrix4x4 toCenter = createTranslationMatrix(-center.x, -center.y, -center.z);
    Matrix4x4 fromCenter = createTranslationMatrix(center.x, center.y, center.z);
    sceneObjects[currentIndex].currentTransformation = fromCenter * transformationMatrix * toCenter * sceneObjects[currentIndex].currentTransformation;
}
