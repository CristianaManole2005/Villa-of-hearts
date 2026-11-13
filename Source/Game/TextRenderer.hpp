#pragma once

// Draws text with glText: https://github.com/vallentin/glText


#include <string>
#include "glew/glew.h"
#include "glm/glm.hpp"

#include "Renderer.hpp"      // programID, MAP_W, drawRect, PANEL color

// glText is a one-file library: this define puts its code here
#define GLT_IMPLEMENTATION
#include "External/gltext.h"

GLTtext* textObject = nullptr;   // reused for every piece of text

// Initialize the text renderer
void initText()
{
	gltInit();
	textObject = gltCreateText();
}

// Delete the text object when closing the game
void deleteText()
{
	gltDeleteText(textObject);
	gltTerminate();
}

// glText works in screen pixels, the game works in map squares
float pixelsPerSquare()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);       // x, y, width, height
	return viewport[2] / MAP_W;					// Calculate how many pixels represent one map unit
}

// Replace apostrophes with a supported character
void setText(const std::string& text)
{
	std::string t = text;
	for (char& c : t) if (c == '\'') c = '`';
	gltSetText(textObject, t.c_str());
}

// "size" sets the text size: a line of text is 5 * size tall
float textScale(float size)
{
	return 5.0f * size * pixelsPerSquare() / gltGetLineHeight(1.0f);
}

// Calculate the width of a text
float textWidth(const std::string& text, float size)
{
	if (text.empty()) return 0.0f;
	setText(text);
	return gltGetTextWidth(textObject, textScale(size)) / pixelsPerSquare();
}

// Draw text at a given position
void drawText(const std::string& text, float x, float y, float size, glm::vec4 color)
{
	if (text.empty()) return;
	float ppu = pixelsPerSquare();
	setText(text);

	gltBeginDraw();													    // Start drawing text
	gltColor(color.r, color.g, color.b, color.a);						// Set the text color
	gltDrawText2D(textObject, x * ppu, y * ppu, textScale(size));		// Draw the text on the screen
	gltEndDraw();														// Finish drawing text
	glUseProgram(programID);   // drawRect / drawCircle use the normal shader
}

// Draw text centered horizontally
void drawTextCentered(const std::string& text, float centerX, float y, float size, glm::vec4 color)
{
	drawText(text, centerX - textWidth(text, size) / 2, y, size, color);
}

// Text on a dark panel, centered horizontally
void drawLabel(const std::string& text, float centerX, float y, float size, glm::vec4 color)
{
	float w = textWidth(text, size);												// Get the text width
	drawRect(centerX - w / 2 - 0.3f, y - 0.25f, w + 0.6f, 5 * size + 0.5f, PANEL);  // Draw the background panel
	drawText(text, centerX - w / 2, y, size, color);								// Draw the text in the center
}
