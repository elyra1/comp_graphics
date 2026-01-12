#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "object.h"
#include <string>

Polyhedron loadOBJ(const std::string& filename);
void saveOBJ(const std::string& filename, const Polyhedron& polyhedron);

#endif
