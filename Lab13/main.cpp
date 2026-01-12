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

// Глобальные переменные для размеров окна
int windowWidth = 1200;
int windowHeight = 1000;

// ID шейдерных программ
GLuint Program;
// ID Uniform-переменных
GLint Unif_MVP;
GLint Unif_texture;
// ID Vertex Array Object
GLuint VAO;
// ID Vertex Buffer Object
GLuint VBO;
// ID буфера для инстансинга
GLuint instanceVBO;
// ID текстуры
GLuint TextureID;

// Количество вершин для отрисовки
int VertexCount = 0;

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
};

// Параметры камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
 #version 330 core
 layout (location = 0) in vec3 coord;
 layout (location = 1) in vec2 in_texCoord;
 // Матрица трансформации конкретного экземпляра
 layout (location = 2) in mat4 instanceMatrix; 
 out vec2 texCoord;
 // Общая матрица проекции и вида
 uniform mat4 u_matrix;
 void main() {
    gl_Position = u_matrix * instanceMatrix * vec4(coord, 1.0);
    texCoord = in_texCoord;
 }
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
 #version 330 core
 precision lowp float;
 in vec2 texCoord;
 out vec4 color;
 uniform sampler2D u_texture;
 void main() {
    color = texture(u_texture, texCoord);
 }
)";

void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}
}

// Загрузка OBJ файла с поддержкой триангуляции многоугольников
bool LoadOBJ(const char* path, std::vector<Vertex>& out_vertices) {
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open OBJ file: " << path << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;
		if (prefix == "v") {
			glm::vec3 v;
			ss >> v.x >> v.y >> v.z;
			temp_vertices.push_back(v);
		}
		else if (prefix == "vt") {
			glm::vec2 uv;
			ss >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (prefix == "f") {
			std::vector<Vertex> faceVertices;
			std::string vertexData;
			while (ss >> vertexData) {
				int vIdx = 0, uvIdx = 0, nIdx = 0;
				if (sscanf(vertexData.c_str(), "%d/%d/%d", &vIdx, &uvIdx, &nIdx) >= 2 ||
					sscanf(vertexData.c_str(), "%d/%d", &vIdx, &uvIdx) >= 2) {
					faceVertices.push_back({ temp_vertices[vIdx - 1], temp_uvs[uvIdx - 1] });
				}
			}
			// Разбиваем многоугольник на треугольники (Fan triangulation), так как в модели полигоны из 4 вершин
			for (size_t i = 1; i < faceVertices.size() - 1; i++) {
				out_vertices.push_back(faceVertices[0]);
				out_vertices.push_back(faceVertices[i]);
				out_vertices.push_back(faceVertices[i + 1]);
			}
		}
	}
	return true;
}

void InitVBO() {
	std::vector<Vertex> vertices;
	if (!LoadOBJ("model.obj", vertices)) {
		std::cout << "Using fallback data..." << std::endl;
		vertices.push_back({ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back({ {1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back({ {0.0f,  1.0f, 0.0f}, {0.5f, 1.0f} });
	}
	VertexCount = (int)vertices.size();

	// 1. Генерируем VAO и VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &instanceVBO);

	// 2. Привязываем VAO
	glBindVertexArray(VAO);

	// 3. Привязываем и наполняем VBO (геометрия загружается один раз)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// 4. Настраиваем атрибуты
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	// 5. Настройка буфера для инстансированных данных (матрицы трансформации)
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	// Выделяем память под 6 экземпляров
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 6, nullptr, GL_DYNAMIC_DRAW);

	// Матрица 4x4 занимает 4 слота атрибутов
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
		//атрибут обновляется раз в экземпляр
		glVertexAttribDivisor(2 + i, 1);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LoadTexture() {
	sf::Image image;
	if (!image.loadFromFile("texture.jpg")) {
		std::cout << "Failed to load texture.jpg!" << std::endl;
		return;
	}
	// Переворачиваем текстуру, так как в OpenGL (0,0) внизу, а в изображениях вверху
	image.flipVertically();

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	//Генерируем Mipmap-ы, чтобы текстура лучше масштабировалась на расстоянии
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

GLuint CreateProgram(const char* vSrc, const char* fSrc) {
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vSrc, NULL);
	glCompileShader(vShader);
	ShaderLog(vShader);

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fSrc, NULL);
	glCompileShader(fShader);
	ShaderLog(fShader);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vShader);
	glAttachShader(prog, fShader);
	glLinkProgram(prog);

	int link_ok;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);
	if (!link_ok) std::cout << "error attach shaders \n";
	return prog;
}

void Init() {
	// Включаем отсечение нелицевых граней 
	glEnable(GL_CULL_FACE);
	// Включаем z-буфер
	glEnable(GL_DEPTH_TEST);

	Program = CreateProgram(VertexShaderSource, FragShaderSource);
	Unif_MVP = glGetUniformLocation(Program, "u_matrix");
	Unif_texture = glGetUniformLocation(Program, "u_texture");

	InitVBO();
	LoadTexture();
}

void Draw() {
	static float angle = 0.0f;
	angle += 0.01f;

	glUseProgram(Program);
	glBindVertexArray(VAO);

	// Привязываем текстуру 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// Указываем шейдеру, что текстура в 0-м слоте
	glUniform1i(Unif_texture, 0);

	float aspect = (float)windowWidth / (float)windowHeight;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 baseOrientation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

	// Устанавливаем общую матрицу (Projection * View)
	glUniformMatrix4fv(Unif_MVP, 1, GL_FALSE, glm::value_ptr(projection * view));

	// Подготавливаем массив трансформаций для ВСЕХ объектов (Солнце + 5 планет = 6 экземпляров)
	std::vector<glm::mat4> instanceMatrices;

	// 1. Трансформация для Солнца
	glm::mat4 modelSun = glm::mat4(1.0f);
	modelSun = glm::rotate(modelSun, angle * 0.5f, glm::vec3(0, 1, 0));
	modelSun = modelSun * baseOrientation;
	modelSun = glm::scale(modelSun, glm::vec3(2.0f));
	instanceMatrices.push_back(modelSun);

	// 2. Трансформации для планет
	for (int i = 1; i <= 5; i++) {
		float orbitRadius = i * 15.0f;
		float orbitSpeed = 1.0f / (float)i;
		float selfRotateSpeed = 2.0f;
		float scale = 0.3f * i;

		glm::mat4 modelPlanet = glm::mat4(1.0f);
		modelPlanet = glm::rotate(modelPlanet, angle * orbitSpeed, glm::vec3(0, 1, 0));
		modelPlanet = glm::translate(modelPlanet, glm::vec3(orbitRadius, 0, 0));
		modelPlanet = glm::rotate(modelPlanet, angle * selfRotateSpeed, glm::vec3(0, 1, 0));
		modelPlanet = modelPlanet * baseOrientation;
		modelPlanet = glm::scale(modelPlanet, glm::vec3(scale));

		instanceMatrices.push_back(modelPlanet);
	}

	// Загружаем данные всех матриц в Instance VBO за один раз
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data());

	// ОДИН вызов отрисовки для всех 6 объектов
	glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, (GLsizei)instanceMatrices.size());
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	glBindVertexArray(0); // Отвязываем VAO

	glUseProgram(0);
}

void ProcessInput(sf::Window& window) {
	float cameraSpeed = 0.15f;
	// Движение вперед-назад
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) cameraPos += cameraSpeed * cameraFront;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) cameraPos -= cameraSpeed * cameraFront;
	// Движение влево-вправо
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	// Движение вверх-вниз
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) cameraPos += cameraSpeed * cameraUp;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) cameraPos -= cameraSpeed * cameraUp;

	// Поворот камеры (стрелками)
	float sensitivity = 1.5f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) yaw -= sensitivity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) yaw += sensitivity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) pitch += sensitivity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) pitch -= sensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void Release() {
	glDeleteProgram(Program);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &TextureID);
}

int main() {
	sf::Window window(sf::VideoMode(windowWidth, windowHeight), "Solar System OpenGL Instanced", sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	glewInit();
	Init();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::Resized) {
				windowWidth = event.size.width;
				windowHeight = event.size.height;
				glViewport(0, 0, windowWidth, windowHeight);
			}
		}

		ProcessInput(window);

		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		// Очищаем и буфер цвета, и буфер глубины
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Draw();

		window.display();
	}

	Release();
	return 0;
}