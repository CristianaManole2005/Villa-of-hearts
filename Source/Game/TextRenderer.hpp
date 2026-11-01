#pragma once
// TextRenderer.hpp  (glText version)
// Draws text with glText: https://github.com/vallentin/glText


#include <string>
#include "glew/glew.h"
#include "glm/glm.hpp"

#include "Renderer.hpp"      // programID, MAP_W, drawRect, PANEL color

// glText is a one-file library: this define puts its code here
#define GLT_IMPLEMENTATION
#include "External/gltext.h"

GLTtext* textObject = nullptr;   // reused for every piece of text

void initText()
{
	gltInit();
	textObject = gltCreateText();
}

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
	return viewport[2] / MAP_W;
}

// glText's font has no apostrophe, so ' is shown with the similar `
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

float textWidth(const std::string& text, float size)
{
	if (text.empty()) return 0.0f;
	setText(text);
	return gltGetTextWidth(textObject, textScale(size)) / pixelsPerSquare();
}

void drawText(const std::string& text, float x, float y, float size, glm::vec4 color)
{
	if (text.empty()) return;
	float ppu = pixelsPerSquare();
	setText(text);

	gltBeginDraw();
	gltColor(color.r, color.g, color.b, color.a);
	gltDrawText2D(textObject, x * ppu, y * ppu, textScale(size));
	gltEndDraw();

	glUseProgram(programID);   // drawRect / drawCircle use the normal shader
}

void drawTextCentered(const std::string& text, float centerX, float y, float size, glm::vec4 color)
{
	drawText(text, centerX - textWidth(text, size) / 2, y, size, color);
}

// Text on a dark panel, centered horizontally
void drawLabel(const std::string& text, float centerX, float y, float size, glm::vec4 color)
{
	float w = textWidth(text, size);
	drawRect(centerX - w / 2 - 0.3f, y - 0.25f, w + 0.6f, 5 * size + 0.5f, PANEL);
	drawText(text, centerX - w / 2, y, size, color);
}
