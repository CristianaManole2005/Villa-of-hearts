#pragma once
// ============================================================
// Renderer.hpp
// OpenGL setup and basic drawing: rectangles, circles, hearts, characters
// (text is in TextRenderer.hpp)
// ============================================================

#include <vector>
#include <string>
#include <cmath>
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Config.hpp"
#include "Colors.hpp"
#include "PixelArt.hpp"

// Shader program, uniform locations and the two shapes everything is made of
GLuint programID;
GLint transformLoc, colorLoc;
glm::mat4 projection;
GLuint quadVao, circleVao;
const int CIRCLE_SEGMENTS = 32;

// ------------------------------------------------------------
// Viewport: keep the 4:3 map ratio (black bars instead of stretching)
// ------------------------------------------------------------
void setViewport(int width, int height)
{
	float target = MAP_W / MAP_H;
	int vw = width;
	int vh = (int)(width / target);
	if (vh > height) {
		vh = height;
		vw = (int)(height * target);
	}
	glViewport((width - vw) / 2, (height - vh) / 2, vw, vh);
}

void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	setViewport(new_width, new_height);
}

// ------------------------------------------------------------
// Geometry: the unit square and unit circle (VAO, VBO, IBO)
// ------------------------------------------------------------
void createQuad()
{
	// Unit square from (0,0) to (1,1); scaled/moved with the model matrix
	GLfloat vertices[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	GLuint indices[] = { 0, 3, 1, 1, 3, 2 };

	GLuint vbo, ibo;
	glGenVertexArrays(1, &quadVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
}

void createCircle()
{
	// Unit circle as a triangle fan: center + points around the edge
	std::vector<GLfloat> v;
	v.push_back(0.0f); v.push_back(0.0f); v.push_back(0.0f);
	for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
		float a = 2.0f * 3.14159265f * i / CIRCLE_SEGMENTS;
		v.push_back(cosf(a)); v.push_back(sinf(a)); v.push_back(0.0f);
	}

	GLuint vbo;
	glGenVertexArrays(1, &circleVao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(circleVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLfloat), v.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
}

// ------------------------------------------------------------
// Drawing helpers (all use the same unit square / unit circle)
// ------------------------------------------------------------
void drawRect(float x, float y, float w, float h, glm::vec4 color,
	glm::mat4 parent = glm::mat4(1.0f))
{
	glm::mat4 model = parent;
	model = glm::translate(model, glm::vec3(x, y, 0.0f));
	model = glm::scale(model, glm::vec3(w, h, 1.0f));
	glm::mat4 mvp = projection * model;

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));
	glBindVertexArray(quadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawCircle(float cx, float cy, float r, glm::vec4 color)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cx, cy, 0.0f));
	model = glm::scale(model, glm::vec3(r, r, 1.0f));
	glm::mat4 mvp = projection * model;

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));
	glBindVertexArray(circleVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS + 2);
}

void drawOutlinedRect(float x, float y, float w, float h, glm::vec4 color)
{
	drawRect(x - 0.1f, y - 0.1f, w + 0.2f, h + 0.2f, OUTLINE);
	drawRect(x, y, w, h, color);
}

void drawHeart(float x, float y, float pixel, glm::vec4 color, bool shine,
	glm::mat4 parent = glm::mat4(1.0f))
{
	for (int row = 0; row < HEART_H; row++)
		for (int col = 0; col < HEART_W; col++) {
			char c = HEART_BITMAP[row][col];
			if (c == '.') continue;
			glm::vec4 pc = (c == 'h' && shine) ? HEART_SHINE : color;
			drawRect(x + col * pixel, y + row * pixel, pixel, pixel, pc, parent);
		}
}

// Pixel character with the given colors. bitmap = CHARACTER_BITMAP (boy)
// or GIRL_BITMAP (girl); both are CHAR_W x CHAR_H
void drawCharacter(glm::vec2 pos, glm::vec4 hair, glm::vec4 top, glm::vec4 bottom,
	const char** bitmap = CHARACTER_BITMAP)
{
	const glm::vec4 skin(0.96f, 0.78f, 0.62f, 1.0f);
	float px = characterSize.x / CHAR_W;
	float py = characterSize.y / CHAR_H;
	for (int row = 0; row < CHAR_H; row++)
		for (int col = 0; col < CHAR_W; col++) {
			char c = bitmap[row][col];
			glm::vec4 color;
			if (c == 'H') color = hair;
			else if (c == 'S') color = skin;
			else if (c == 'P') color = top;
			else if (c == 'B') color = bottom;
			else continue;
			drawRect(pos.x + col * px, pos.y + row * py, px, py, color);
		}
}
