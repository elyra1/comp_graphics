#include "objloader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

Polyhedron loadOBJ(const std::string &filename)
{
    std::vector<Point3D> vertices;
    std::vector<Polygon> polygons;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            double x, y, z;
            ss >> x >> y >> z;
            vertices.push_back(Point3D(x, y, z));
        }
        else if (type == "f")
        {
            std::vector<Point3D> polygon_points;
            std::string vertex_str;
            while (ss >> vertex_str)
            {
                std::stringstream vertex_ss(vertex_str);
                int index;
                vertex_ss >> index;
                polygon_points.push_back(vertices[index - 1]);
            }
            polygons.push_back(Polygon(polygon_points));
        }
    }

    return Polyhedron(polygons);
}

void saveOBJ(const std::string &filename, const Polyhedron &polyhedron)
{
    std::ofstream file(filename);
    std::map<Point3D, int> vertex_to_index;
    std::vector<Point3D> unique_vertices;

    int current_index = 1;

    for (const auto &polygon : polyhedron.polygons)
    {
        for (const auto &point : polygon.points)
        {
            if (vertex_to_index.find(point) == vertex_to_index.end())
            {
                vertex_to_index[point] = current_index++;
                unique_vertices.push_back(point);
            }
        }
    }

    for (const auto &vertex : unique_vertices)
    {
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }

    for (const auto &polygon : polyhedron.polygons)
    {
        file << "f";
        for (const auto &point : polygon.points)
        {
            file << " " << vertex_to_index[point];
        }
        file << std::endl;
    }
}
