#include "Common.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

int windowWidth = 1200;
int windowHeight = 1000;

GLuint Program;

// Конфигурации света
struct PointLight {
    bool enabled = true;
    glm::vec4 position = { 5.0f, 25.0f, 0.0f, 1.0f };
    glm::vec4 ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
    glm::vec4 diffuse = { 0.9f, 0.9f, 0.9f, 1.0f };
    glm::vec4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec3 attenuation = { 1.0f, 0.007f, 0.0002f };

    void Reset() {
        enabled = true;
        position = { 5.0f, 25.0f, 0.0f, 1.0f };
        ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
        diffuse = { 0.9f, 0.9f, 0.9f, 1.0f };
        specular = { 1.0f, 1.0f, 1.0f, 1.0f };
        attenuation = { 1.0f, 0.007f, 0.0002f };
    }
} pointLight;

struct DirLight {
    bool enabled = true;
    glm::vec3 direction = { -0.3f, -1.0f, -0.5f };
    glm::vec4 ambient = { 0.1f, 0.1f, 0.15f, 1.0f };
    glm::vec4 diffuse = { 0.4f, 0.4f, 0.4f, 1.0f };

    void Reset() {
        enabled = true;
        direction = { -0.3f, -1.0f, -0.5f };
        ambient = { 0.1f, 0.1f, 0.15f, 1.0f };
        diffuse = { 0.4f, 0.4f, 0.4f, 1.0f };
    }
} dirLight;

struct SpotLight {
    bool enabled = true;
    float cutoff = 0.95f;
    float exponent = 50.0f;
    glm::vec3 attenuation = { 1.0f, 0.007f, 0.0002f };
    glm::vec4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 diffuse = { 1.0f, 1.0f, 0.8f, 1.0f };

    void Reset() {
        enabled = true;
        cutoff = 0.95f;
        exponent = 50.0f;
        specular = { 1.0f, 1.0f, 1.0f, 1.0f };
        diffuse = { 1.0f, 1.0f, 0.8f, 1.0f };
        attenuation = { 1.0f, 0.007f, 0.0002f };
    }
} spotLight;

std::vector<SceneObject> sceneObjects;

glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.2f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;

void Init() {
    glEnable(GL_DEPTH_TEST);
    Program = CreateProgram(VertexShaderSource, FragShaderSource);

    AddObject("Table", "table.obj", "table.jpg",
        glm::vec3(0, 0, 0), glm::vec3(0.14), glm::vec3(0, 90, 0),
        0,
        glm::vec4(0.2, 0.15, 0.1, 1.0),
        glm::vec4(0.6, 0.45, 0.3, 1.0),
        glm::vec4(0.3, 0.3, 0.3, 1.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        5.0f);

    AddObject("Chair", "chair.obj", "chair.jpg",
        glm::vec3(23, -20, 0), glm::vec3(1.2), glm::vec3(0, 90, 0),
        0,
        glm::vec4(0.2, 0.15, 0.1, 1.0),
        glm::vec4(0.6, 0.45, 0.3, 1.0),
        glm::vec4(0.3, 0.3, 0.3, 1.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        1.0f);

    AddObject("Monitor", "monitor.obj", "monitor.jpg",
        glm::vec3(-7, 10.0f, -2.0f), glm::vec3(0.7), glm::vec3(-90.0f, 0, 90),
        0,
        glm::vec4(0.1, 0.1, 0.1, 1.0),
        glm::vec4(0.3, 0.3, 0.3, 1.0),
        glm::vec4(0.8, 0.8, 0.8, 1.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        100.0f);

    AddObject("Headphones", "headphone.obj", "headphone.jpg",
        glm::vec3(19, 15.0f, 0.0f), glm::vec3(1), glm::vec3(0, 90, 0),
        2,
        glm::vec4(0.1, 0.1, 0.1, 1.0),
        glm::vec4(0.4, 0.4, 0.4, 1.0),
        glm::vec4(0.7, 0.7, 0.7, 1.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        64.0f);

    AddObject("Cat", "cat.obj", "cat.jpg",
        glm::vec3(22, 3, 0), glm::vec3(1.7), glm::vec3(-90.0f, 0, -90),
        0,
        glm::vec4(0.2, 0.2, 0.2, 1.0),
        glm::vec4(0.8, 0.8, 0.8, 1.0),
        glm::vec4(0.1, 0.1, 0.1, 1.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        8.0f);
}

void Draw() {
    if (sceneObjects.empty()) return;

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glUseProgram(Program);

    // Вспомогательная переменная для отключения света (черный цвет)
    glm::vec4 offColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform3f(glGetUniformLocation(Program, "u_viewPosition"), cameraPos.x, cameraPos.y, cameraPos.z);


    // Передача Point Light
    glUniform4fv(glGetUniformLocation(Program, "light.position"), 1, glm::value_ptr(pointLight.position));
    glUniform4fv(glGetUniformLocation(Program, "light.ambient"), 1, glm::value_ptr(pointLight.enabled ? pointLight.ambient : offColor));
    glUniform4fv(glGetUniformLocation(Program, "light.diffuse"), 1, glm::value_ptr(pointLight.enabled ? pointLight.diffuse : offColor));
    glUniform4fv(glGetUniformLocation(Program, "light.specular"), 1, glm::value_ptr(pointLight.enabled ? pointLight.specular : offColor));
    glUniform3fv(glGetUniformLocation(Program, "light.attenuation"), 1, glm::value_ptr(pointLight.attenuation));

    // Передача Directional Light
    glUniform3fv(glGetUniformLocation(Program, "dirLight.direction"), 1, glm::value_ptr(dirLight.direction));
    glUniform4fv(glGetUniformLocation(Program, "dirLight.ambient"), 1, glm::value_ptr(dirLight.enabled ? dirLight.ambient : offColor));
    glUniform4fv(glGetUniformLocation(Program, "dirLight.diffuse"), 1, glm::value_ptr(dirLight.enabled ? dirLight.diffuse : offColor));

    // Передача Spot Light
    glUniform3f(glGetUniformLocation(Program, "spotLight.position"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(glGetUniformLocation(Program, "spotLight.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
    glUniform1f(glGetUniformLocation(Program, "spotLight.cutoff"), spotLight.cutoff);
    glUniform1f(glGetUniformLocation(Program, "spotLight.exponent"), spotLight.exponent);
    glUniform4fv(glGetUniformLocation(Program, "spotLight.diffuse"), 1, glm::value_ptr(spotLight.enabled ? spotLight.diffuse : offColor));
    glUniform4fv(glGetUniformLocation(Program, "spotLight.specular"), 1, glm::value_ptr(spotLight.enabled ? spotLight.specular : offColor));
    glUniform3fv(glGetUniformLocation(Program, "spotLight.attenuation"), 1, glm::value_ptr(spotLight.attenuation));

    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 500.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    for (auto& obj : sceneObjects) {
        glBindTexture(GL_TEXTURE_2D, obj.texture);
        glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

        glUniformMatrix4fv(glGetUniformLocation(Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(obj.modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(Program, "transform.viewProjection"), 1, GL_FALSE, glm::value_ptr(projection * view));

        glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(obj.modelMatrix)));
        glUniformMatrix3fv(glGetUniformLocation(Program, "transform.normal"), 1, GL_FALSE, glm::value_ptr(normMat));

        glUniform3f(glGetUniformLocation(Program, "transform.viewPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform1i(glGetUniformLocation(Program, "u_lightModel"), obj.defaultModel);
        glUniform1i(glGetUniformLocation(Program, "material.texture"), 0);
        glUniform4f(glGetUniformLocation(Program, "material.ambient"), obj.matAmbient.x, obj.matAmbient.y, obj.matAmbient.z, obj.matAmbient.w);
        glUniform4f(glGetUniformLocation(Program, "material.diffuse"), obj.matDiffuse.x, obj.matDiffuse.y, obj.matDiffuse.z, obj.matDiffuse.w);
        glUniform4f(glGetUniformLocation(Program, "material.specular"), obj.matSpecular.x, obj.matSpecular.y, obj.matSpecular.z, obj.matSpecular.w);
        glUniform4f(glGetUniformLocation(Program, "material.emission"), obj.matEmission.x, obj.matEmission.y, obj.matEmission.z, obj.matEmission.w);
        glUniform1f(glGetUniformLocation(Program, "material.shininess"), obj.matShininess);

        glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void ProcessInput(sf::Window& window) {
    if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse) return;

    float cameraSpeed = 0.5f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) cameraPos += cameraSpeed * cameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) cameraPos -= cameraSpeed * cameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float sensitivity = 1.2f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) yaw -= sensitivity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) yaw += sensitivity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) pitch += sensitivity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) pitch -= sensitivity;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void RenderGui() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);

    ImGui::Begin("Lighting System");

    // --- POINT LIGHT ---
    if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enabled point", &pointLight.enabled);
        ImGui::SameLine();
        if (ImGui::Button("Reset##PL")) { pointLight.Reset(); }

        ImGui::BeginDisabled(!pointLight.enabled);
        ImGui::DragFloat3("Position", glm::value_ptr(pointLight.position), 0.2f);
        ImGui::ColorEdit4("Ambient##PL", glm::value_ptr(pointLight.ambient));
        ImGui::ColorEdit4("Diffuse##PL", glm::value_ptr(pointLight.diffuse));
        ImGui::ColorEdit4("Specular##PL", glm::value_ptr(pointLight.specular));
        ImGui::Text("Point Attenuation:");
        ImGui::SliderFloat("Linear", &pointLight.attenuation.y, 0.0f, 0.1f, "%.4f");
        ImGui::SliderFloat("Quadratic", &pointLight.attenuation.z, 0.0f, 0.01f, "%.5f");
        ImGui::EndDisabled();
    }

    ImGui::Separator();

    // --- DIRECTIONAL LIGHT ---
    if (ImGui::CollapsingHeader("Directional Light")) {
        ImGui::Checkbox("Enabled dir", &dirLight.enabled);
        ImGui::SameLine();
        if (ImGui::Button("Reset##DL")) { dirLight.Reset(); }

        ImGui::BeginDisabled(!dirLight.enabled);
        ImGui::DragFloat3("Direction", glm::value_ptr(dirLight.direction), 0.02f, -1.0f, 1.0f);
        ImGui::ColorEdit4("Ambient##DL", glm::value_ptr(dirLight.ambient));
        ImGui::ColorEdit4("Diffuse##DL", glm::value_ptr(dirLight.diffuse));
        ImGui::EndDisabled();
    }

    ImGui::Separator();

    // --- SPOT LIGHT ---
    if (ImGui::CollapsingHeader("Flashlight (Spot light)")) {
        ImGui::Checkbox("Enabled flashlight", &spotLight.enabled);
        ImGui::SameLine();
        if (ImGui::Button("Reset##SL")) { spotLight.Reset(); }

        ImGui::BeginDisabled(!spotLight.enabled);
        ImGui::SliderFloat("Cutoff (Width)", &spotLight.cutoff, 0.5f, 1.0f);
        ImGui::SliderFloat("Exponent (Softness)", &spotLight.exponent, 0.0f, 128.0f);
        ImGui::ColorEdit4("Color##SL", glm::value_ptr(spotLight.diffuse));
        ImGui::ColorEdit4("Spot Specular##PL", glm::value_ptr(spotLight.specular));
        ImGui::Text("Spot Attenuation:");
        ImGui::SliderFloat("Linear", &spotLight.attenuation.y, 0.0f, 0.1f, "%.4f");
        ImGui::SliderFloat("Quadratic", &spotLight.attenuation.z, 0.0f, 0.01f, "%.5f");
        ImGui::EndDisabled();
    }

    ImGui::End();
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML + ImGui Light Control", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    glewInit();
    if (!ImGui::SFML::Init(window)) return -1;

    Init();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                windowWidth = event.size.width;
                windowHeight = event.size.height;
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ProcessInput(window);
        RenderGui();

        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.pushGLStates(); 
        ImGui::SFML::Render(window);
        window.popGLStates(); 

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}