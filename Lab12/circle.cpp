#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Circle {

	// ID шейдерной программы
	GLuint Program;
	// ID вершинных атрибутов
	GLint Attrib_vertex;
	// ID атрибутов цвета
	GLint Attrib_color;
	// ID Uniform-переменной масштаба
	GLint Unif_matrix;
	// ID Vertex Buffer Object
	GLuint VBO;

	// Переменные для управления масштабом
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	struct Vertex {
		GLfloat x, y;       // Координаты
		GLfloat r, g, b;    // Цвет
	};

	// Вспомогательная функция для перевода HSV в RGB (S=1, V=1)
	void hsvToRgb(float h, float& r, float& g, float& b) {
		float f = h / 60.0f;
		float x = (1.0f - std::abs(std::fmod(f, 2.0f) - 1.0f));
		if (0 <= f && f < 1) { r = 1; g = x; b = 0; }
		else if (1 <= f && f < 2) { r = x; g = 1; b = 0; }
		else if (2 <= f && f < 3) { r = 0; g = 1; b = x; }
		else if (3 <= f && f < 4) { r = 0; g = x; b = 1; }
		else if (4 <= f && f < 5) { r = x; g = 0; b = 1; }
		else if (5 <= f && f <= 6) { r = 1; g = 0; b = x; }
		else { r = 0; g = 0; b = 0; }
	}

	// Исходный код вершинного шейдера
	const char* VertexShaderSource = R"(
	#version 330 core
	in vec2 coord;    // Координаты вершины
	in vec3 in_color; // Цвет вершины
	out vec3 vert_color;
	uniform mat4 u_matrix;

	void main() {
		gl_Position = u_matrix * vec4(coord, 0.0, 1.0);
    	vert_color = in_color;
	}
	)";

	// Исходный код фрагментного шейдера
	const char* FragShaderSource = R"(
	 #version 330 core
	 precision lowp float;
	 in vec3 vert_color;
	 out vec4 color;
	 void main() {
		color = vec4(vert_color, 1.0);
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

	void InitVBO() {
		glGenBuffers(1, &VBO);

		std::vector<Vertex> vertices;
		const int points = 100; // Количество сегментов круга
		float radius = 0.5f;

		// Центральная точка (белая)
		vertices.push_back({ 0.0f, 0.0f, 1.0f, 1.0f, 1.0f });

		for (int i = 0; i <= points; i++) {
			float angle = 2.0f * 3.14159265f * i / points;
			float hue = 360.0f * i / points;

			float r, g, b;
			hsvToRgb(hue, r, g, b);

			float vx = radius * cos(angle);
			float vy = radius * sin(angle);

			vertices.push_back({ vx, vy, r, g, b });
		}

		// активизируем буфер
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// Передаем вершины в буфер
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	GLuint CreateProgram(const char* vSrc, const char* fSrc) {
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vSrc, NULL);
		glCompileShader(vShader);
		std::cout << "vertex shader \n";
		ShaderLog(vShader);

		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fSrc, NULL);
		glCompileShader(fShader);
		std::cout << "fragment shader \n";
		ShaderLog(fShader);

		GLuint prog = glCreateProgram();
		glAttachShader(prog, vShader);
		glAttachShader(prog, fShader);
		glLinkProgram(prog);

		int link_ok;
		glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			std::cout << "error attach shaders \n";
		}
		return prog;
	}

	void Init() {
		Program = CreateProgram(VertexShaderSource, FragShaderSource);

		Attrib_vertex = glGetAttribLocation(Program, "coord");
		Attrib_color = glGetAttribLocation(Program, "in_color");
		Unif_matrix = glGetUniformLocation(Program, "u_matrix");

		InitVBO();
	}

	void Draw() {
		glUseProgram(Program);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
		glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, glm::value_ptr(model));

		glEnableVertexAttribArray(Attrib_vertex);
		glEnableVertexAttribArray(Attrib_color);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Указываем формат данных (координаты + цвет)
		glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(GLfloat)));

		// Рисуем круг (центр + 101 точка по окружности = 102 вершины)
		glDrawArrays(GL_TRIANGLE_FAN, 0, 102);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(Attrib_vertex);
		glDisableVertexAttribArray(Attrib_color);
		glUseProgram(0);
	}

	void ReleaseVBO() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);
	}

	void ReleaseShader() {
		glUseProgram(0);
		glDeleteProgram(Program);
	}

	void Release() {
		ReleaseShader();
		ReleaseVBO();
	}

	int run() {
		sf::Window window(sf::VideoMode(600, 600), "Circle", sf::Style::Default, sf::ContextSettings(24));
		window.setVerticalSyncEnabled(true);
		window.setActive(true);

		glewInit();
		Init();

		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) { window.close(); }
				else if (event.type == sf::Event::Resized) { glViewport(0, 0, event.size.width, event.size.height); }
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) scaleX -= 0.01f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) scaleX += 0.01f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) scaleY += 0.01f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) scaleY -= 0.01f;

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();

			window.display();
		}
		Release();
		return 0;
	}
}