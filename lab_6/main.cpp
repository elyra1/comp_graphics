#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <variant>

#include "geometry.h"
#include "object.h"
#include "shapes.h"
#include "renderer.h"
#include "input.h"

Polyhedron currentPolyhedron;
float rotationX = 0.0f;
float rotationY = 0.0f;
bool perspectiveProjection = true;
Matrix4x4 currentTransformation;
std::string currentMode = "VIEW";
unsigned int windowWidth = 800;
unsigned int windowHeight = 600;

int main()
{
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Lab 6 - 3D Transformations with SFML");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") && !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    {
        std::cerr << "Error loading font\n";
        return -1;
    }

    currentPolyhedron = createTetrahedron();
    currentTransformation.identity();

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) 
            {
                windowWidth = resized->size.x;
                windowHeight = resized->size.y;
                sf::FloatRect visibleArea({0.f, 0.f}, {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
                window.setView(sf::View(visibleArea));
            }

            // Передача события в обработчик ввода
            handleEvent(*event);
        }

        display(window, font);
    }

    return 0;
}
