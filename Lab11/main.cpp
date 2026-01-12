#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// Перечисление для режимов закрашивания
enum FillMode {
	CONSTANT = 0, // Цвет константой в шейдере
	UNIFORM = 1,  // Цвет через uniform-переменную
	GRADIENT = 2  // Градиент по вершинам
};

// Выбранный режим (измените это значение для смены режима закрашивания)
FillMode currentMode = GRADIENT;

// ID шейдерных программ
GLuint Programs[3];
// ID вершинных атрибутов
GLint Attrib_vertex[3];
// ID атрибутов цвета
GLint Attrib_color[3];
// ID Uniform-переменных
GLint Unif_color;
// ID Vertex Buffer Object
GLuint VBO;

struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
 #version 330 core
 in vec2 coord;
 in vec3 in_color;
 out vec3 vert_color;
 void main() {
 gl_Position = vec4(coord, 0.0, 1.0);
 vert_color = in_color;
 }
)";

// Исходный код фрагментного шейдера (Константа)
const char* FragShaderConstSource = R"(
 #version 330 core
 precision lowp float;
 out vec4 color;
 void main() {
 color = vec4(1.0, 0.5, 0.0, 1.0); 
 }
)";

// Исходный код фрагментного шейдера (Uniform)
const char* FragShaderUnifSource = R"(
 #version 330 core
 precision lowp float;
 uniform vec4 u_color;
 out vec4 color;
 void main() {
 color = u_color;
 }
)";

// Исходный код фрагментного шейдера (Градиент)
const char* FragShaderGradSource = R"(
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

	// 1. Четырехугольник (4 вершины)
	vertices.push_back({ -0.8f,  0.8f,  1.0f, 0.0f, 0.0f });
	vertices.push_back({ -0.2f,  0.8f,  0.0f, 1.0f, 0.0f });
	vertices.push_back({ -0.2f,  0.3f,  0.0f, 0.0f, 1.0f });
	vertices.push_back({ -0.8f,  0.3f,  1.0f, 1.0f, 0.0f });

	// 2. Веер (5 вершин)
	vertices.push_back({ 0.5f,  0.4f,  1.0f, 1.0f, 1.0f }); // Центр
	vertices.push_back({ 0.3f,  0.8f,  1.0f, 0.0f, 0.0f });
	vertices.push_back({ 0.7f,  0.8f,  0.0f, 1.0f, 0.0f });
	vertices.push_back({ 0.8f,  0.5f,  0.0f, 0.0f, 1.0f });
	vertices.push_back({ 0.7f,  0.2f,  1.0f, 0.0f, 1.0f });

	// 3. Правильный пятиугольник 
	float centerX = 0.0f, centerY = -0.5f, radius = 0.3f;
	vertices.push_back({ centerX, centerY, 1.0f, 1.0f, 1.0f }); // Центр пятиугольника
	for (int i = 0; i <= 5; i++) {
		float angle = 2.0f * 3.14159f * i / 5.0f;
		vertices.push_back({
			centerX + radius * cos(angle),
			centerY + radius * sin(angle),
			(float)(i % 2), (float)((i + 1) % 2), 0.5f
			});
	}

	//активизируем буфер
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Передаем вершины в буфер
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

GLuint CreateProgram(const char* vSrc, const char* fSrc) {
	// Создаем вершинный шейдер
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// Передаем исходный код
	glShaderSource(vShader, 1, &vSrc, NULL);
	// Компилируем шейдер
	glCompileShader(vShader);
	std::cout << "vertex shader \n";
	// Функция печати лога шейдера
	ShaderLog(vShader);

	// Создаем фрагментный шейдер
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Передаем исходный код
	glShaderSource(fShader, 1, &fSrc, NULL);
	// Компилируем шейдер
	glCompileShader(fShader);
	std::cout << "fragment shader \n";
	// Функция печати лога шейдера
	ShaderLog(fShader);

	// Создаем программу и прикрепляем шейдеры к ней
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vShader);
	glAttachShader(prog, fShader);
	// Линкуем шейдерную программу
	glLinkProgram(prog);

	// Проверяем статус сборки
	int link_ok;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		std::cout << "error attach shaders \n";
	}
	return prog;
}

void Init() {
	// Шейдеры
	Programs[CONSTANT] = CreateProgram(VertexShaderSource, FragShaderConstSource);
	Programs[UNIFORM] = CreateProgram(VertexShaderSource, FragShaderUnifSource);
	Programs[GRADIENT] = CreateProgram(VertexShaderSource, FragShaderGradSource);

	// Вытягиваем ID атрибутов и uniform из собранных программ
	for (int i = 0; i < 3; i++) {
		Attrib_vertex[i] = glGetAttribLocation(Programs[i], "coord");
		Attrib_color[i] = glGetAttribLocation(Programs[i], "in_color");
	}
	Unif_color = glGetUniformLocation(Programs[UNIFORM], "u_color");

	// Вершинный буфер
	InitVBO();
}

void Draw() {
	glUseProgram(Programs[currentMode]); // Устанавливаем шейдерную программу текущей

	// Передаем цвет (для режима UNIFORM)
	if (currentMode == UNIFORM) {
		glUniform4f(Unif_color, 0.0f, 0.8f, 0.8f, 1.0f); // Бирюзовый для режима UNIFORM
	}

	glEnableVertexAttribArray(Attrib_vertex[currentMode]); // Включаем массив атрибутов координат
	glEnableVertexAttribArray(Attrib_color[currentMode]);  // Включаем массив атрибутов цвета

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Указываем формат данных (координаты + цвет)
	glVertexAttribPointer(Attrib_vertex[currentMode], 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(Attrib_color[currentMode], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(GLfloat)));

	// Рисуем четырехугольник (первые 4 вершины)
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// Рисуем веер (следующие 5 вершин)
	glDrawArrays(GL_TRIANGLE_FAN, 4, 5);

	// Рисуем пятиугольник (оставшиеся 7 вершин)
	glDrawArrays(GL_TRIANGLE_FAN, 9, 7);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO
	glDisableVertexAttribArray(Attrib_vertex[currentMode]);
	glDisableVertexAttribArray(Attrib_color[currentMode]);
	glUseProgram(0); // Отключаем шейдерную программу
}

// Освобождение буфера
void ReleaseVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
}

// Освобождение шейдеров
void ReleaseShader() {
	// Передавая ноль, мы отключаем шейдерную программу
	glUseProgram(0);
	// Удаляем шейдерные программы
	for (int i = 0; i < 3; i++) {
		glDeleteProgram(Programs[i]);
	}
}

void Release() {
	// Шейдеры
	ReleaseShader();
	// Вершинный буфер
	ReleaseVBO();
}

int main() {
	sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Draw();
		window.display();
	}
	Release();
	return 0;
}