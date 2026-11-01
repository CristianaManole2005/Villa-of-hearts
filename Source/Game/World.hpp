#pragma once
// ============================================================
// World.hpp
// Static world: buildings, trees, fire pit, and shape overlap tests
// ============================================================

#include <vector>
#include <string>
#include <cmath>
#include "glm/glm.hpp"

// ------------------------------------------------------------
// Basic types
// ------------------------------------------------------------
struct Box {
	float x, y, w, h;
	Box() : x(0), y(0), w(0), h(0) {}
	Box(float x_, float y_, float w_, float h_) : x(x_), y(y_), w(w_), h(h_) {}
};

struct Tree {
	glm::vec2 pos;       // trunk base
	Tree(float x, float y) : pos(x, y) {}
};

// ------------------------------------------------------------
// World objects
// ------------------------------------------------------------
std::vector<Tree> trees = {
	Tree(1.5f, 24.0f), Tree(38.5f, 24.0f), Tree(8.5f, 6.5f),
	Tree(34.0f, 12.0f), Tree(38.5f, 11.0f), Tree(1.5f, 16.0f),
	Tree(9.0f, 24.0f), Tree(34.5f, 22.0f), Tree(38.5f, 19.0f),
	Tree(6.5f, 9.5f)
};

std::vector<Box> solids = {
	Box(12.0f, 2.0f, 16.0f, 8.0f),     // main house
	Box(15.0f, 14.0f, 10.0f, 5.0f),    // pool
	Box(31.0f, 3.0f, 6.0f, 5.0f),      // hideaway
	Box(31.0f, 14.0f, 6.0f, 4.0f)      // day beds
};
const glm::vec2 firePitCenter(7.0f, 16.5f);
const float firePitRadius = 3.0f;

// Overlap tests between shapes
bool rectsOverlap(glm::vec2 p, glm::vec2 s, const Box& b)
{
	return p.x < b.x + b.w && p.x + s.x > b.x &&
		p.y < b.y + b.h && p.y + s.y > b.y;
}

bool rectInside(glm::vec2 p, glm::vec2 s, const Box& b)
{
	return p.x >= b.x && p.y >= b.y && p.x + s.x <= b.x + b.w && p.y + s.y <= b.y + b.h;
}

// Circle vs rectangle: find the closest point of the rect to the circle center
bool circleRectOverlap(glm::vec2 center, float radius, glm::vec2 p, glm::vec2 s)
{
	float closestX = glm::clamp(center.x, p.x, p.x + s.x);
	float closestY = glm::clamp(center.y, p.y, p.y + s.y);
	glm::vec2 diff = center - glm::vec2(closestX, closestY);
	return glm::dot(diff, diff) < radius * radius;
}
