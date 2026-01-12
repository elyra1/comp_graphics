#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Tetrahedron {

	// ID шейдерной программы
	GLuint Program;
	// ID вершинных атрибутов
	GLint Attrib_vertex;
	// ID атрибутов цвета
	GLint Attrib_color;
	// ID Uniform-переменной для матрицы вращения
	GLint Unif_matrix;
	// ID Vertex Buffer Object
	GLuint VBO;

	struct Vertex {
		GLfloat x, y, z;    // Координаты (3D)
		GLfloat r, g, b;    // Цвета (Градиент)
	};

	// Переменные для управления позицией
	float curX = 0.0f;
	float curY = 0.0f;
	float curZ = 0.0f;

	// Переменные для управления вращением
	float rotX = 0.4f;
	float rotY = 0.4f;

	// Исходный код вершинного шейдера
	const char* VertexShaderSource = R"(
	 #version 330 core
	 in vec3 coord;
	 in vec3 in_color;

	 uniform mat4 u_matrix;
	 out vec3 vert_color;

	 void main() {
		 // Применяем смещение и вращения из uniform
		 gl_Position = u_matrix * vec4(coord, 1.0);
		 vert_color = in_color;
	 }
)";

	// Исходный код фрагментного шейдера (Градиент)
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

		// Координаты для правильного тетраэдра
		float s = 0.5f;
		// Вершины: Координаты(x,y,z), Цвет(r,g,b)
		Vertex v0 = { s,  s,  s,   1.0f, 0.0f, 0.0f };
		Vertex v1 = { -s, -s,  s,   0.0f, 1.0f, 0.0f };
		Vertex v2 = { -s,  s, -s,   0.0f, 0.0f, 1.0f };
		Vertex v3 = { s, -s, -s,   1.0f, 1.0f, 0.0f };

		// Грань 1
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
		// Грань 2
		vertices.push_back(v0); vertices.push_back(v2); vertices.push_back(v3);
		// Грань 3
		vertices.push_back(v0); vertices.push_back(v3); vertices.push_back(v1);
		// Грань 4
		vertices.push_back(v1); vertices.push_back(v3); vertices.push_back(v2);

		// активизируем буфер
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// Передаем вершины в буфер (координаты и цвета)
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	GLuint CreateProgram(const char* vSrc, const char* fSrc) {
		// Создаем вершинный шейдер
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vSrc, NULL);
		glCompileShader(vShader);
		std::cout << "vertex shader \n";
		ShaderLog(vShader);

		// Создаем фрагментный шейдер
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fSrc, NULL);
		glCompileShader(fShader);
		std::cout << "fragment shader \n";
		ShaderLog(fShader);

		// Создаем программу
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
		// Включаем тест глубины для 3D
		glEnable(GL_DEPTH_TEST);

		// Шейдеры
		Program = CreateProgram(VertexShaderSource, FragShaderSource);

		// Вытягиваем ID атрибутов и uniform из программы
		Attrib_vertex = glGetAttribLocation(Program, "coord");
		Attrib_color = glGetAttribLocation(Program, "in_color");
		Unif_matrix = glGetUniformLocation(Program, "u_matrix");

		// Вершинный буфер
		InitVBO();
	}

	void Draw() {
		glUseProgram(Program); // Устанавливаем шейдерную программу текущей

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(curX, curY, curZ));
		model = glm::rotate(model, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotY, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, glm::value_ptr(model));

		glEnableVertexAttribArray(Attrib_vertex);
		glEnableVertexAttribArray(Attrib_color);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Указываем формат данных согласно структуре
		glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));

		// Рисуем тетраэдр (4 грани по 3 вершины)
		glDrawArrays(GL_TRIANGLES, 0, 12);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO
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

	// Обработка ввода
	void HandleInput() {
		float step = 0.01f;
		float rotSpeed = 0.02f;

		// Вращение WASD
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) rotX -= rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) rotX += rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) rotY -= rotSpeed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) rotY += rotSpeed;

		// Перемещение стрелками
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  curX -= step;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) curX += step;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    curY += step;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  curY -= step;
	}

	int run() {
		sf::Window window(sf::VideoMode(600, 600), "Tetrahedron", sf::Style::Default, sf::ContextSettings(24));
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

			HandleInput();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			// Очистка буфера цвета и глубины
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();
			window.display();
		}

		Release();
		return 0;
	}

}