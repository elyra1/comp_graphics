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
#include "input.h"
#include "objloader.h"
#include "surface.h"
#include "gui.h"
#include "camera.h"

Camera camera;
std::vector<SceneObject> sceneObjects;
int currentIndex = 0;
bool perspectiveProjection = true;
std::string currentMode = "VIEW";
unsigned int windowWidth = 800;
unsigned int windowHeight = 600;

int main(int argc, char *argv[])
{
    SceneObject initialObject;
    initialObject.polyhedron = createTetrahedron();
    initialObject.currentTransformation.identity();
    sceneObjects.push_back(initialObject);

    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Lab 8");
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window, false))
    {
        std::cerr << "Error initializing ImGui-SFML\n";
        return -1;
    }
    ImGui::GetIO().Fonts->AddFontDefault();
    ImGui::SFML::UpdateFontTexture();
    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") && !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    {
        std::cerr << "Error loading font\n";
        return -1;
    }

    if (argc > 1)
    {
        sceneObjects[currentIndex].polyhedron = loadOBJ(argv[1]);
    }
    else
    {
        sceneObjects[currentIndex].polyhedron = createTetrahedron();
    }
    sceneObjects[currentIndex].polyhedron.calculateNormals();
    sceneObjects[currentIndex].currentTransformation.identity();

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
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                windowWidth = resized->size.x;
                windowHeight = resized->size.y;
                sf::FloatRect visibleArea({0.f, 0.f}, {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
                window.setView(sf::View(visibleArea));
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        drawGui();

        window.clear(sf::Color::White);
        display(window, font, sceneObjects);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
