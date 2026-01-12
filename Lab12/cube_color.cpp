#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CubeColor {

	// ID шейдерной программы
	GLuint Program;
	// ID атрибутов
	GLint Attrib_vertex;
	GLint Attrib_color;
	GLint Attrib_texCoord;
	// ID Uniform-переменных
	GLint Unif_matrix;
	GLint Unif_influence;
	GLint Unif_texture;
	// ID Vertex Buffer Object
	GLuint VBO;
	// ID Текстуры
	GLuint TextureID;

	// Переменные состояния
	float colorInfluence = 0.5f; // Влияние цвета (Up/Down)
	float rotX = 0.4f;           // Поворот вокруг X (W/S)
	float rotY = 0.4f;           // Поворот вокруг Y (A/D)

	struct Vertex {
		GLfloat x, y, z;    // Координаты
		GLfloat r, g, b;    // Цвет
		GLfloat u, v;       // Текстурные координаты
	};

	// Исходный код вершинного шейдера
	const char* VertexShaderSource = R"(
	#version 330 core
	in vec3 coord;
	in vec3 in_color;
	in vec2 in_texCoord;
	out vec3 vert_color;
	out vec2 vert_texCoord;
	uniform mat4 u_matrix;
	void main() {
		gl_Position = u_matrix * vec4(coord, 1.0);
		vert_color = in_color;
		vert_texCoord = in_texCoord;
	}
	)";

	// Исходный код фрагментного шейдера
	const char* FragShaderSource = R"(
	#version 330 core
	precision lowp float;
	in vec3 vert_color;
	in vec2 vert_texCoord;
	out vec4 color;
	uniform sampler2D u_texture;
	uniform float u_influence;
	void main() {
		vec4 texColor = texture(u_texture, vert_texCoord);
		color = mix(texColor, vec4(vert_color, 1.0), u_influence);
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

		std::vector<Vertex> vertices = {
			// Передняя грань
			{ -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f }, // Красный
			{  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f }, // Зеленый
			{  0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f }, // Синий
			{ -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f }, // Желтый

			// Задняя грань
			{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f }, // Бирюзовый
			{ -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f }, // Пурпурный
			{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f }, // Белый
			{  0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  0.0f, 1.0f }, // Оранжевый

			// Верхняя грань
			{ -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f }, // Пурпурный
			{ -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f }, // Желтый
			{  0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f }, // Синий
			{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f }, // Белый

			// Нижняя грань
			{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f }, // Бирюзовый
			{  0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  0.0f, 0.0f }, // Оранжевый
			{  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f }, // Зеленый
			{ -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f }, // Красный

			// Правая грань
			{  0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  1.0f, 1.0f }, // Оранжевый
			{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f }, // Белый
			{  0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f }, // Синий
			{  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f }, // Зеленый

			// Левая грань
			{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f }, // Бирюзовый
			{ -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f }, // Красный
			{ -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f }, // Желтый
			{ -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f }  // Пурпурный
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void InitTexture() {
		sf::Image image;
		if (!image.loadFromFile("texture1.png")) {
			// Если файла нет, создаем "шахматку" программно
			image.create(128, 128, sf::Color::Cyan);
		}

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
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

		return prog;
	}

	void Init() {
		glEnable(GL_DEPTH_TEST);

		Program = CreateProgram(VertexShaderSource, FragShaderSource);

		Attrib_vertex = glGetAttribLocation(Program, "coord");
		Attrib_color = glGetAttribLocation(Program, "in_color");
		Attrib_texCoord = glGetAttribLocation(Program, "in_texCoord");

		Unif_matrix = glGetUniformLocation(Program, "u_matrix");
		Unif_influence = glGetUniformLocation(Program, "u_influence");
		Unif_texture = glGetUniformLocation(Program, "u_texture");

		InitVBO();
		InitTexture();
	}

	void HandleInput() {
		float rotSpeed = 0.02f;
		float influenceSpeed = 0.01f;

		// Вращение WASD
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) rotX -= rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) rotX += rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) rotY -= rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) rotY += rotSpeed;

		// Смешивание цвета (Up/Down)
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			colorInfluence += influenceSpeed;
			if (colorInfluence > 1.0f) colorInfluence = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			colorInfluence -= influenceSpeed;
			if (colorInfluence < 0.0f) colorInfluence = 0.0f;
		}
	}

	void Draw() {
		glUseProgram(Program);
		// 1. Матрица модели (вращение)
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotY, glm::vec3(0.0f, 1.0f, 0.0f));

		// 2. Проекция (чтобы куб имел объем/перспективу)
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

		// 3. Вид (отодвинем камеру назад, чтобы видеть куб)
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

		// Итоговая матрица
		glm::mat4 mvp = projection * view * model;

		glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1f(Unif_influence, colorInfluence);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glUniform1i(Unif_texture, 0);

		glEnableVertexAttribArray(Attrib_vertex);
		glEnableVertexAttribArray(Attrib_color);
		glEnableVertexAttribArray(Attrib_texCoord);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Указываем формат данных согласно структуре
		glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(Attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));

		for (int i = 0; i < 6; i++) {
			glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(Attrib_vertex);
		glDisableVertexAttribArray(Attrib_color);
		glDisableVertexAttribArray(Attrib_texCoord);
		glUseProgram(0);
	}

	void Release() {
		glDeleteProgram(Program);
		glDeleteTextures(1, &TextureID);
		glDeleteBuffers(1, &VBO);
	}

	int run() {
		sf::Window window(sf::VideoMode(800, 800), "Cube color", sf::Style::Default, sf::ContextSettings(24));
		window.setVerticalSyncEnabled(true);
		window.setActive(true);

		glewInit();
		Init();

		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::Resized) glViewport(0, 0, event.size.width, event.size.height);
			}

			HandleInput();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();
			window.display();
		}

		Release();
		return 0;
	}
}
