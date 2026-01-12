#include "input.h"
#include "renderer.h"
#include <iostream>
#include <variant>

void applyTransformation(const Matrix4x4 &transformation)
{
    currentTransformation = transformation * currentTransformation;
}

void applyTransformationAroundPolyhedronCenter(const Matrix4x4 &transformation)
{
    Point3D center = currentTransformation.transform(currentPolyhedron.getCenter());
    Matrix4x4 T = createTranslationMatrix(-center.x, -center.y, -center.z);
    Matrix4x4 T_inv = createTranslationMatrix(center.x, center.y, center.z);
    applyTransformation(T_inv * transformation * T);
}