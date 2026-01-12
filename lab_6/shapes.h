#ifndef SHAPES_H
#define SHAPES_H

#include "object.h"
#include "geometry.h"

Polyhedron createPolyhedronFromVerticesAndFaces(const std::vector<Point3D>& vertices, const std::vector<std::vector<int>>& faces);

Polyhedron createTetrahedron();
Polyhedron createHexahedron();
Polyhedron createOctahedron();
Polyhedron createIcosahedron();
Polyhedron createDodecahedron();

#endif