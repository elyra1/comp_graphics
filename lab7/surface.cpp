#include "surface.h"
#include <cmath>
#include <vector>

float evaluate_function(int func_type, float x, float y)
{
    switch (func_type)
    {
    case 0:
        return sin(x) * cos(y);
    case 1:
        return x * x + y * y;
    case 2:
        return std::sqrt(x * x + y * y);
    default:
        return 0.0f;
    }
}

Polyhedron generateSurface(int func_type, float x_0, float x_1, float y_0, float y_1, int x_steps, int y_steps)
{
    std::vector<Point3D> vertices;
    std::vector<Polygon> polygons;

    float dx = (x_1 - x_0) / x_steps;
    float dy = (y_1 - y_0) / y_steps;
    for (int i = 0; i <= x_steps; ++i)
    {
        for (int j = 0; j <= y_steps; ++j)
        {
            float x = x_0 + i * dx;
            float y = y_0 + j * dy;
            float z = evaluate_function(func_type, x, y);
            vertices.push_back({x, y, z});
        }
    }
    for (int i = 0; i < x_steps; ++i)
    {
        for (int j = 0; j < y_steps; ++j)
        {
            int v1_idx = i * (y_steps + 1) + j;
            int v2_idx = (i + 1) * (y_steps + 1) + j;
            int v3_idx = (i + 1) * (y_steps + 1) + (j + 1);
            int v4_idx = i * (y_steps + 1) + (j + 1);

            std::vector<Point3D> quad_points = {
                vertices[v1_idx],
                vertices[v2_idx],
                vertices[v3_idx],
                vertices[v4_idx]};
            polygons.push_back(Polygon(quad_points));
        }
    }

    return Polyhedron(polygons);
}
