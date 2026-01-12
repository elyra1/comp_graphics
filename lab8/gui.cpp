#include "gui.h"
#include "input.h"
#include "objloader.h"
#include "surface.h"
#include "renderer.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include "object.h"
#include <vector>

extern std::vector<SceneObject> sceneObjects;
extern int currentIndex;

extern bool perspectiveProjection;
extern std::string currentMode;

static char objFilePathBuffer[256] = "utah_teapot_lowpoly.obj";

static std::vector<Point3D> generatingPoints = {{0.5, -0.5, 0}, {0.5, 0.5, 0}};
static int revolutionSteps = 12;
static int selectedAxis = 1;
static char revolutionObjPathBuffer[256] = "revolution.obj";

static int surfaceFuncType = 0;
static float surfaceXMin = -5.0f;
static float surfaceXMax = 5.0f;
static float surfaceYMin = -5.0f;
static float surfaceYMax = 5.0f;
static int surfaceXSteps = 20;
static int surfaceYSteps = 20;
static char surfaceFilePathBuffer[256] = "surface.obj";

void clearScene()
{
    sceneObjects.clear();
    currentIndex = -1;
    perspectiveProjection = true;
    currentMode = "object";
}

void drawGui()
{
    ImGui::Begin("Controls");

    if (ImGui::CollapsingHeader("Object Management"))
    {
        if (ImGui::Button("Add New Object"))
        {
            sceneObjects.emplace_back();
            currentIndex = sceneObjects.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Scene"))
        {
            clearScene();
        }

        if (sceneObjects.empty())
        {
            ImGui::Text("No objects in scene.");
        }
        else
        {
            std::vector<std::string> objectNames;
            for (int i = 0; i < sceneObjects.size(); ++i)
            {
                objectNames.push_back("Object " + std::to_string(i));
            }
            std::string preview = "Object " + std::to_string(currentIndex);
            if (ImGui::BeginCombo("Select Object", preview.c_str()))
            {
                for (int i = 0; i < objectNames.size(); ++i)
                {
                    const bool isSelected = (currentIndex == i);
                    if (ImGui::Selectable(objectNames[i].c_str(), isSelected))
                    {
                        currentIndex = i;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    if (ImGui::CollapsingHeader("Polyhedron Selection"))
    {
        if (ImGui::Button("Tetrahedron"))
        {
            sceneObjects[currentIndex].polyhedron = createTetrahedron();
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }
        ImGui::SameLine();
        if (ImGui::Button("Hexahedron"))
        {
            sceneObjects[currentIndex].polyhedron = createHexahedron();
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }
        ImGui::SameLine();
        if (ImGui::Button("Octahedron"))
        {
            sceneObjects[currentIndex].polyhedron = createOctahedron();
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }
        if (ImGui::Button("Icosahedron"))
        {
            sceneObjects[currentIndex].polyhedron = createIcosahedron();
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }
        ImGui::SameLine();
        if (ImGui::Button("Dodecahedron"))
        {
            sceneObjects[currentIndex].polyhedron = createDodecahedron();
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }
    }

    // Affine Transformations
    if (ImGui::CollapsingHeader("Affine Transformations"))
    {
        ImGui::Text("Translation");
        ImGui::SameLine();
        if (ImGui::Button("Tx+"))
            applyTransformation(createTranslationMatrix(0.5, 0, 0));
        ImGui::SameLine();
        if (ImGui::Button("Tx-"))
            applyTransformation(createTranslationMatrix(-0.5, 0, 0));
        ImGui::SameLine();
        if (ImGui::Button("Ty+"))
            applyTransformation(createTranslationMatrix(0, 0.5, 0));
        ImGui::SameLine();
        if (ImGui::Button("Ty-"))
            applyTransformation(createTranslationMatrix(0, -0.5, 0));
        ImGui::SameLine();
        if (ImGui::Button("Tz+"))
            applyTransformation(createTranslationMatrix(0, 0, 0.5));
        ImGui::SameLine();
        if (ImGui::Button("Tz-"))
            applyTransformation(createTranslationMatrix(0, 0, -0.5));

        ImGui::Text("Scaling");
        ImGui::SameLine();
        if (ImGui::Button("S+"))
            applyTransformation(createScaleMatrix(1.2, 1.2, 1.2));
        ImGui::SameLine();
        if (ImGui::Button("S-"))
            applyTransformation(createScaleMatrix(0.8, 0.8, 0.8));

        ImGui::Text("Rotation");
        ImGui::SameLine();
        if (ImGui::Button("Rx+"))
            applyTransformation(createRotationXMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("Rx-"))
            applyTransformation(createRotationXMatrix(-15));
        ImGui::SameLine();
        if (ImGui::Button("Ry+"))
            applyTransformation(createRotationYMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("Ry-"))
            applyTransformation(createRotationYMatrix(-15));
        ImGui::SameLine();
        if (ImGui::Button("Rz+"))
            applyTransformation(createRotationZMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("Rz-"))
            applyTransformation(createRotationZMatrix(-15));

        if (ImGui::Button("Arbitrary Rotation +15"))
        {
            Point3D p1(1, 1, 1);
            Point3D p2(-1, -1, -1);
            applyTransformation(createArbitraryRotationMatrix(p1, p2, 15.0));
        }
        ImGui::SameLine();
        if (ImGui::Button("Arbitrary Rotation -15"))
        {
            Point3D p1(1, 1, 1);
            Point3D p2(-1, -1, -1);
            applyTransformation(createArbitraryRotationMatrix(p1, p2, -15.0));
        }
    }

    // Transformations Around Center
    if (ImGui::CollapsingHeader("Transformations Around Center"))
    {
        ImGui::Text("Scaling Around Center");
        ImGui::SameLine();
        if (ImGui::Button("CS+"))
            applyTransformationAroundPolyhedronCenter(createScaleMatrix(1.5, 1.5, 1.5));
        ImGui::SameLine();
        if (ImGui::Button("CS-"))
            applyTransformationAroundPolyhedronCenter(createScaleMatrix(0.7, 0.7, 0.7));

        ImGui::Text("Rotation Around Center");
        ImGui::SameLine();
        if (ImGui::Button("CRx+"))
            applyTransformationAroundPolyhedronCenter(createRotationXMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("CRx-"))
            applyTransformationAroundPolyhedronCenter(createRotationXMatrix(-15));
        ImGui::SameLine();
        if (ImGui::Button("CRy+"))
            applyTransformationAroundPolyhedronCenter(createRotationYMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("CRy-"))
            applyTransformationAroundPolyhedronCenter(createRotationYMatrix(-15));
        ImGui::SameLine();
        if (ImGui::Button("CRz+"))
            applyTransformationAroundPolyhedronCenter(createRotationZMatrix(15));
        ImGui::SameLine();
        if (ImGui::Button("CRz-"))
            applyTransformationAroundPolyhedronCenter(createRotationZMatrix(-15));
    }

    if (ImGui::CollapsingHeader("Reflection"))
    {
        if (ImGui::Button("Reflect X"))
            applyTransformation(createReflectionMatrix(ReflectionPlane::X));
        ImGui::SameLine();
        if (ImGui::Button("Reflect Y"))
            applyTransformation(createReflectionMatrix(ReflectionPlane::Y));
        ImGui::SameLine();
        if (ImGui::Button("Reflect Z"))
            applyTransformation(createReflectionMatrix(ReflectionPlane::Z));
    }

    // Projection
    if (ImGui::CollapsingHeader("Projection"))
    {
        ImGui::Checkbox("Perspective Projection", &perspectiveProjection);
    }

    if (ImGui::CollapsingHeader("Rendering"))
    {
        ImGui::Checkbox("Backface Culling", &enableBackfaceCulling);
        ImGui::Checkbox("Enable Z-Buffer", &enableZBuffer);
    }

    if (ImGui::CollapsingHeader("OBJ Loading/Saving"))
    {
        ImGui::InputText("File Path", objFilePathBuffer, IM_ARRAYSIZE(objFilePathBuffer));
        ImGui::SameLine();
        if (ImGui::Button("Load OBJ"))
        {
            try
            {
                sceneObjects[currentIndex].polyhedron = loadOBJ(objFilePathBuffer);
                sceneObjects[currentIndex].polyhedron.calculateNormals();
                sceneObjects[currentIndex].currentTransformation.identity();
                std::cout << "Loaded " << objFilePathBuffer << std::endl;
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << "Error loading OBJ: " << e.what() << std::endl;
            }
        }

        if (ImGui::Button("Save OBJ (output.obj)"))
        {
            Polyhedron transformedPolyhedron = sceneObjects[currentIndex].polyhedron;
            transformedPolyhedron.transform(sceneObjects[currentIndex].currentTransformation);
            saveOBJ("output.obj", transformedPolyhedron);
            std::cout << "Saved to output.obj" << std::endl;
        }
    }

    if (ImGui::CollapsingHeader("Figure of Revolution"))
    {
        ImGui::Text("Generating Points");
        for (size_t i = 0; i < generatingPoints.size(); ++i)
        {
            ImGui::PushID(i);
            float p[3] = {(float)generatingPoints[i].x, (float)generatingPoints[i].y, (float)generatingPoints[i].z};
            if (ImGui::InputFloat3("##point", p))
            {
                generatingPoints[i].x = p[0];
                generatingPoints[i].y = p[1];
                generatingPoints[i].z = p[2];
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                generatingPoints.erase(generatingPoints.begin() + i);
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Add Point"))
        {
            generatingPoints.push_back(Point3D(0, 0, 0));
        }

        ImGui::Text("Rotation Axis");
        ImGui::RadioButton("X", &selectedAxis, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Y", &selectedAxis, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Z", &selectedAxis, 2);

        ImGui::InputInt("Steps", &revolutionSteps);
        ImGui::InputText("Output Filename", revolutionObjPathBuffer, IM_ARRAYSIZE(revolutionObjPathBuffer));

        if (ImGui::Button("Generate and Save"))
        {
            if (generatingPoints.size() >= 2 && revolutionSteps > 2)
            {
                char axis = 'x';
                if (selectedAxis == 1)
                    axis = 'y';
                if (selectedAxis == 2)
                    axis = 'z';

                Polyhedron revolutionFigure = createFigureOfRevolution(generatingPoints, axis, revolutionSteps);
                saveOBJ(revolutionObjPathBuffer, revolutionFigure);
                std::cout << "Generated and saved figure of revolution to " << revolutionObjPathBuffer << std::endl;
            }
            else
            {
                std::cerr << "Error: Need at least 2 points and 3 steps to create a figure." << std::endl;
            }
        }
    }

    // Surface Generation
    if (ImGui::CollapsingHeader("Surface Generation"))
    {
        const char *items[] = {"sin(x) * cos(y)", "x^2 + y^2", "sqrt(x^2 + y^2)"};
        ImGui::Combo("Function", &surfaceFuncType, items, IM_ARRAYSIZE(items));

        ImGui::InputFloat("X Min", &surfaceXMin);
        ImGui::InputFloat("X Max", &surfaceXMax);
        ImGui::InputFloat("Y Min", &surfaceYMin);
        ImGui::InputFloat("Y Max", &surfaceYMax);
        ImGui::InputInt("X Steps", &surfaceXSteps);
        ImGui::InputInt("Y Steps", &surfaceYSteps);

        if (ImGui::Button("Generate Surface"))
        {
            sceneObjects[currentIndex].polyhedron = generateSurface(surfaceFuncType, surfaceXMin, surfaceXMax, surfaceYMin, surfaceYMax, surfaceXSteps, surfaceYSteps);
            sceneObjects[currentIndex].polyhedron.calculateNormals();
            sceneObjects[currentIndex].currentTransformation.identity();
        }

        ImGui::InputText("Surface File Path", surfaceFilePathBuffer, IM_ARRAYSIZE(surfaceFilePathBuffer));
        if (ImGui::Button("Save Surface"))
        {
            Polyhedron transformedPolyhedron = sceneObjects[currentIndex].polyhedron;
            transformedPolyhedron.transform(sceneObjects[currentIndex].currentTransformation);
            saveOBJ(surfaceFilePathBuffer, transformedPolyhedron);
            std::cout << "Saved surface to " << surfaceFilePathBuffer << std::endl;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Surface"))
        {
            try
            {
                sceneObjects[currentIndex].polyhedron = loadOBJ(surfaceFilePathBuffer);
                sceneObjects[currentIndex].polyhedron.calculateNormals();
                sceneObjects[currentIndex].currentTransformation.identity();
                std::cout << "Loaded " << surfaceFilePathBuffer << std::endl;
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << "Error loading surface: " << e.what() << std::endl;
            }
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::IsAnyItemActive())
    {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        camera.orbit(-delta.x, delta.y);
    }

    if (ImGui::Button("Reset Transformations"))
    {
        sceneObjects[currentIndex].currentTransformation.identity();
    }

    ImGui::End();
}
