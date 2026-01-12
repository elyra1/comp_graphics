#include <iostream>

// Объявляем функции из других файлов
namespace Tetrahedron { int run(); }
namespace CubeColor { int run(); }
namespace CubeMulti { int run(); }
namespace Circle { int run(); }

enum LabPart {
    TETRAHEDRON,
    CUBE_COLOR,
    CUBE_MULTI,
    CIRCLE
};

LabPart currentPart = CIRCLE;

int main() {
    switch (currentPart) {
    case TETRAHEDRON: return Tetrahedron::run();
    case CUBE_COLOR:  return CubeColor::run();
    case CUBE_MULTI:  return CubeMulti::run();
    case CIRCLE:      return Circle::run();
    }
    return 0;
}