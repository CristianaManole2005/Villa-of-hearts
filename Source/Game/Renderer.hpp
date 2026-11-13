#pragma once

// OpenGL setup, drawRect, drawCircle, drawHeart... (Functions for drawing shapes)

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
GLuint quadVao, circleVao;		// VAOs for the rectangle and circle
const int CIRCLE_SEGMENTS = 32;	// Number of segments used to draw a circle

// Set the viewport without stretching the map
void setViewport(int width, int height)
{
	float target = MAP_W / MAP_H;     // Calculate the map ratio
	int vw = width;
	int vh = (int)(width / target);

	// Adjust the viewport to fit the window
	if (vh > height) {
		vh = height;
		vw = (int)(height * target);
	}
	glViewport((width - vw) / 2, (height - vh) / 2, vw, vh); // Center the viewport
}

// Update the viewport when resizing the window
void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	setViewport(new_width, new_height);
}

// Create a rectangle using vertices and indices
void createQuad()
{
	// Vertices of a square
	GLfloat vertices[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	GLuint indices[] = { 0, 3, 1, 1, 3, 2 }; // Indices for the two triangles

	// Create VAO, VBO and IBO
	GLuint vbo, ibo;
	glGenVertexArrays(1, &quadVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(quadVao);	// Bind the VAO

	// Store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Store the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set the vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
}

void createCircle()
{
	// Unit circle as a triangle fan: center + points around the edge
	std::vector<GLfloat> v;
	v.push_back(0.0f); v.push_back(0.0f); v.push_back(0.0f);

	// Calculate points around the circle
	for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
		float a = 2.0f * 3.14159265f * i / CIRCLE_SEGMENTS;
		v.push_back(cosf(a)); v.push_back(sinf(a)); v.push_back(0.0f);
	}

	// Create VAO and VBO
	GLuint vbo;
	glGenVertexArrays(1, &circleVao);
	glGenBuffers(1, &vbo);

	// Store the circle vertices
	glBindVertexArray(circleVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLfloat), v.data(), GL_STATIC_DRAW);

	// Set the vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
}

// Draw a rectangle
void drawRect(float x, float y, float w, float h, glm::vec4 color,
	glm::mat4 parent = glm::mat4(1.0f))
{
	glm::mat4 model = parent;								// Create the model matrix
	model = glm::translate(model, glm::vec3(x, y, 0.0f));   // Move the rectangle to its position
	model = glm::scale(model, glm::vec3(w, h, 1.0f));       // Change the size of the rectangle
	glm::mat4 mvp = projection * model;						// Apply the projection

	// Send the matrix and color to the shader
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));

	// Draw the rectangle
	glBindVertexArray(quadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Draw a circle
void drawCircle(float cx, float cy, float r, glm::vec4 color)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cx, cy, 0.0f)); // Set the circle position
	model = glm::scale(model, glm::vec3(r, r, 1.0f));							// Set the circle size
	glm::mat4 mvp = projection * model;											// Apply the projection

	// Send the matrix and color to the shader
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));

	// Draw the circle
	glBindVertexArray(circleVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS + 2);
}

// Draw a rectangle with an outline
void drawOutlinedRect(float x, float y, float w, float h, glm::vec4 color)
{
	drawRect(x - 0.1f, y - 0.1f, w + 0.2f, h + 0.2f, OUTLINE); // Draw a larger rectangle for the outline
	drawRect(x, y, w, h, color);							   // Draw the main rectangle
}

// Draw a heart using pixel art
void drawHeart(float x, float y, float pixel, glm::vec4 color, bool shine,
	glm::mat4 parent = glm::mat4(1.0f))
{
	// Go through each pixel of the heart
	for (int row = 0; row < HEART_H; row++)
		for (int col = 0; col < HEART_W; col++) {
			char c = HEART_BITMAP[row][col];										// Get the current pixel
			if (c == '.') continue;													// Skip empty pixels
			glm::vec4 pc = (c == 'h' && shine) ? HEART_SHINE : color;				// Use a lighter color for the shiny part
			drawRect(x + col * pixel, y + row * pixel, pixel, pixel, pc, parent);   // Draw the pixel as a small rectangle
		}
}

// Draw the player or an NPC using pixel art
void drawCharacter(glm::vec2 pos, glm::vec4 hair, glm::vec4 top, glm::vec4 bottom,
	const char** bitmap = CHARACTER_BITMAP)
{
	const glm::vec4 skin(0.96f, 0.78f, 0.62f, 1.0f);						// Skin color

	// Calculate the size of each pixel
	float px = characterSize.x / CHAR_W;
	float py = characterSize.y / CHAR_H;

	// Go through each pixel of the character
	for (int row = 0; row < CHAR_H; row++)
		for (int col = 0; col < CHAR_W; col++) {
			char c = bitmap[row][col];
			glm::vec4 color;

			// Choose the color based on the letter
			if (c == 'H') color = hair;
			else if (c == 'S') color = skin;
			else if (c == 'P') color = top;
			else if (c == 'B') color = bottom;
			else continue;
			drawRect(pos.x + col * px, pos.y + row * py, px, py, color);	// Draw the pixel
		}
}
