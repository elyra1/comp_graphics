#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum LightingModel {
    PHONG = 0,
    TOON = 1,
    MINNAERT = 2
};

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

struct SceneObject {
    GLuint vbo;
    GLuint texture;
    int vertexCount;
    glm::mat4 modelMatrix;
    int defaultModel;
    std::string name;

    glm::vec4 matAmbient;
    glm::vec4 matDiffuse;
    glm::vec4 matSpecular;
    glm::vec4 matEmission;
    float matShininess;
};


extern int windowWidth;
extern int windowHeight;
extern GLuint Program;
extern std::vector<SceneObject> sceneObjects;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern float yaw;
extern float pitch;

extern const char* VertexShaderSource;
extern const char* FragShaderSource;


void ShaderLog(unsigned int shader);
bool LoadOBJ(const char* path, std::vector<Vertex>& out_vertices);
GLuint LoadTexture(const char* path);
void AddObject(std::string name, const char* modelPath, const char* texPath,
    glm::vec3 pos, glm::vec3 scale, glm::vec3 rotDegrees,
    int modelType,
    glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec4 emission, float shininess);
GLuint CreateProgram(const char* vSrc, const char* fSrc);