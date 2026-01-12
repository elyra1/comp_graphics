#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include <variant>

#include "geometry.h"
#include "object.h"
#include "shapes.h"
#include "renderer.h"
#include "input.h" // Keep this for global variables
#include "objloader.h"
#include "surface.h"
#include "gui.h" // New header for GUI functions

Polyhedron currentPolyhedron;
float rotationX = 0.0f;
float rotationY = 0.0f;
bool perspectiveProjection = true;
Matrix4x4 currentTransformation;
std::string currentMode = "VIEW";
unsigned int windowWidth = 800;
unsigned int windowHeight = 600;

int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Lab 7 - OBJ Viewer");
    window.setFramerateLimit(60);
                if (!ImGui::SFML::Init(window, false)) // Pass false to prevent default font loading by imgui-sfml
                {
                    std::cerr << "Error initializing ImGui-SFML\n";
                    return -1;
                }
                ImGui::GetIO().Fonts->AddFontDefault(); // Load default font
                ImGui::SFML::UpdateFontTexture(); // Explicitly update font texture
        sf::Font font;
        if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") && !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
        {
            std::cerr << "Error loading font\n";
            return -1;
        }
    
        if (argc > 1) {
            currentPolyhedron = loadOBJ(argv[1]);
        } else {
            currentPolyhedron = createTetrahedron();
        }
        currentTransformation.identity();
    
        sf::Clock deltaClock;
        while (window.isOpen())
        {
            while (auto event = window.pollEvent())
            {
                ImGui::SFML::ProcessEvent(window, *event);
    
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
            }
        ImGui::SFML::Update(window, deltaClock.restart());

        drawGui(); // Call the new GUI drawing function

        window.clear(sf::Color::White);
        display(window, font);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
