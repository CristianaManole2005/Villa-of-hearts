#pragma once

// Drawing the map, challenge objects, characters and the UI

#include <vector>
#include <string>
#include <cmath>
#include "glm/glm.hpp"

#include "Renderer.hpp"
#include "TextRenderer.hpp"
#include "World.hpp"
#include "Challenges.hpp"
#include "Npcs.hpp"
#include "GameState.hpp"
#include "GameLogic.hpp"

// Shapes used by the challenges


// Bianca's hidden objects, drawn from simple shapes
void drawHiddenObject(int index, glm::vec2 c, float alpha)
{
	if (index == 0) {          // shell
		drawCircle(c.x, c.y, 0.45f, glm::vec4(1.0f, 0.75f, 0.70f, alpha));
		drawRect(c.x - 0.45f, c.y, 0.9f, 0.45f, glm::vec4(1.0f, 0.62f, 0.58f, alpha));
		for (int i = -1; i <= 1; i++)
			drawRect(c.x + i * 0.2f - 0.03f, c.y - 0.35f, 0.06f, 0.75f, glm::vec4(0.8f, 0.45f, 0.45f, alpha));
	}
	else if (index == 1) {     // sunglasses
		drawCircle(c.x - 0.3f, c.y, 0.25f, glm::vec4(0.1f, 0.1f, 0.12f, alpha));
		drawCircle(c.x + 0.3f, c.y, 0.25f, glm::vec4(0.1f, 0.1f, 0.12f, alpha));
		drawRect(c.x - 0.1f, c.y - 0.05f, 0.2f, 0.08f, glm::vec4(0.1f, 0.1f, 0.12f, alpha));
	}
	else {                     // flower
		for (int i = 0; i < 5; i++) {
			float a = i * 2.0f * 3.14159265f / 5.0f;
			drawCircle(c.x + 0.25f * cosf(a), c.y + 0.25f * sinf(a), 0.2f, glm::vec4(1.0f, 0.5f, 0.75f, alpha));
		}
		drawCircle(c.x, c.y, 0.14f, glm::vec4(1.0f, 0.9f, 0.3f, alpha));
	}
}

// Crab: body, eyes, claws and legs that wiggle while it moves
void drawCrab(glm::vec2 c, float time)
{
	float wiggle = 0.06f * sinf(time * 20.0f);
	for (int i = -1; i <= 1; i++) {
		drawRect(c.x - 0.65f, c.y + i * 0.18f + wiggle, 0.3f, 0.06f, CRAB_COLOR);
		drawRect(c.x + 0.35f, c.y + i * 0.18f - wiggle, 0.3f, 0.06f, CRAB_COLOR);
	}
	drawCircle(c.x - 0.6f, c.y - 0.45f, 0.18f, CRAB_COLOR);      // claws
	drawCircle(c.x + 0.6f, c.y - 0.45f, 0.18f, CRAB_COLOR);
	drawCircle(c.x, c.y, 0.38f, OUTLINE);
	drawCircle(c.x, c.y, 0.33f, CRAB_COLOR);
	drawRect(c.x - 0.15f, c.y - 0.45f, 0.08f, 0.12f, OUTLINE);   // eyes
	drawRect(c.x + 0.07f, c.y - 0.45f, 0.08f, 0.12f, OUTLINE);
}

// Two small footprints
void drawFootprints(glm::vec2 c, glm::vec4 color)
{
	drawCircle(c.x - 0.18f, c.y + 0.1f, 0.14f, color);
	drawCircle(c.x - 0.18f, c.y - 0.12f, 0.1f, color);
	drawCircle(c.x + 0.18f, c.y - 0.15f, 0.14f, color);
	drawCircle(c.x + 0.18f, c.y - 0.37f, 0.1f, color);
}

// Map
void drawMap(float time)
{
	// Grass with small dark dots for texture
	drawRect(0, 0, MAP_W, 26, GRASS);
	for (int y = 0; y < 26; y++)
		for (int x = 0; x < 40; x++)
			if ((x * 7 + y * 13) % 5 == 0)
				drawRect(x + 0.3f, y + 0.4f, 0.2f, 0.2f, GRASS_DARK);

	// Beach and animated sea
	drawRect(0, 26, MAP_W, 2, SAND);
	drawRect(0, 28, MAP_W, 2, SEA);
	for (int i = 0; i < 8; i++) {
		float wx = fmodf(i * 5.0f + time * 1.5f, MAP_W + 3.0f) - 3.0f;
		drawRect(wx, 28.5f + (i % 2) * 0.8f, 2.0f, 0.15f, SEA_LIGHT);
	}

	// Paths
	drawRect(2.0f, 7.0f, 1.0f, 4.9f, PATH);
	drawRect(2.0f, 10.9f, 18.5f, 1.0f, PATH);
	drawRect(19.5f, 11.9f, 1.0f, 2.1f, PATH);
	drawRect(10.0f, 16.0f, 5.0f, 1.0f, PATH);
	drawRect(25.0f, 16.0f, 6.0f, 1.0f, PATH);
	drawRect(19.5f, 19.0f, 1.0f, 7.0f, PATH);
	drawRect(28.0f, 4.5f, 3.0f, 1.0f, PATH);

	// Gate
	drawOutlinedRect(1.0f, 4.0f, 3.0f, 3.0f, STONE);
	drawRect(2.0f, 4.0f, 1.0f, 3.0f, WOOD_DARK);

	// Main house
	drawOutlinedRect(12, 2, 16, 8, WALL);
	drawRect(12, 2, 16, 1.2f, ROOF);
	for (int i = 0; i < 4; i++)
		drawOutlinedRect(13.5f + i * 3.8f, 4.2f, 2.0f, 1.8f, GLASS);
	drawRect(19.0f, 8.3f, 2.0f, 1.7f, glm::vec4(0.75f, 0.22f, 0.17f, 1.0f));

	// Terrace
	drawRect(14, 10, 12, 2, WOOD);
	for (int i = 0; i < 12; i++)
		drawRect(14.0f + i, 10, 0.08f, 2, WOOD_DARK);

	// Pool
	drawRect(14.7f, 13.7f, 10.6f, 5.6f, WALL);
	drawRect(15, 14, 10, 5, POOL);
	drawRect(16.0f, 15.0f + 0.3f * sinf(time * 2.0f), 3.0f, 0.2f, SEA_LIGHT);
	drawRect(21.0f, 17.0f + 0.3f * sinf(time * 2.0f + 1.5f), 3.0f, 0.2f, SEA_LIGHT);

	// Fire pit with flickering fire
	drawCircle(firePitCenter.x, firePitCenter.y, firePitRadius, OUTLINE);
	drawCircle(firePitCenter.x, firePitCenter.y, firePitRadius - 0.15f, STONE);
	float flicker = 1.0f + 0.15f * sinf(time * 12.0f) + 0.08f * sinf(time * 23.0f);
	drawCircle(firePitCenter.x, firePitCenter.y, 1.0f * flicker, FIRE);
	drawCircle(firePitCenter.x, firePitCenter.y, 0.5f * flicker, FIRE_YELLOW);

	// Hideaway and day beds
	drawOutlinedRect(31, 3, 6, 5, PINK);
	drawRect(32, 4, 4, 3, glm::vec4(1.0f, 0.85f, 0.90f, 1.0f));
	drawOutlinedRect(31, 14, 6, 4, AMBER);
	drawRect(31.5f, 14.5f, 2.3f, 3.0f, WALL);
	drawRect(34.2f, 14.5f, 2.3f, 3.0f, WALL);

	// Obstacle course: ropes, start pad and checkered finish line
	glm::vec4 rope(0.95f, 0.95f, 0.95f, 0.8f);
	for (float x = COURSE.x; x < COURSE.x + COURSE.w; x += 0.8f) {
		drawRect(x, COURSE.y - 0.1f, 0.5f, 0.12f, rope);
		drawRect(x, COURSE.y + COURSE.h - 0.02f, 0.5f, 0.12f, rope);
	}
	drawRect(COURSE.x, COURSE.y, 1.5f, COURSE.h, glm::vec4(0.45f, 0.85f, 0.45f, 0.5f));
	for (int row = 0; row < 11; row++)
		for (int col = 0; col < 4; col++)
			drawRect(COURSE_FINISH.x + col * 0.5f, COURSE_FINISH.y + row * 0.5f, 0.5f, 0.5f,
				(row + col) % 2 == 0 ? glm::vec4(0.1f, 0.1f, 0.1f, 0.7f) : glm::vec4(1.0f, 1.0f, 1.0f, 0.7f));

	// Palm trees
	for (const Tree& t : trees) {
		drawRect(t.pos.x - 0.2f, t.pos.y - 1.5f, 0.4f, 1.5f, TRUNK);
		drawCircle(t.pos.x, t.pos.y - 1.8f, 1.0f, LEAF);
		drawCircle(t.pos.x - 0.25f, t.pos.y - 2.0f, 0.55f, LEAF_LIGHT);
	}
}

// Drawing the challenges and the UI

void drawChallengeObjects(float time)
{
	if (state != CHALLENGE) return;
	glm::vec2 pc = playerCenter();
	float pulse = 0.5f + 0.5f * sinf(time * 6.0f);

	switch (activeChallenge) {
	case OBSTACLE_COURSE:
		for (int i = 0; i < BALL_COUNT; i++) {
			glm::vec2 b = ballPosition(i, time);
			drawCircle(b.x, b.y, BALL_RADIUS, OUTLINE);
			drawCircle(b.x, b.y, BALL_RADIUS - 0.08f, TEXT_WHITE);
			drawRect(b.x - 0.5f, b.y - 0.12f, 1.0f, 0.24f, i % 2 ? HEART_BRIGHT : POOL);
			drawCircle(b.x, b.y, 0.12f, TEXT_YELLOW);
		}
		break;

	case POOL_CORNERS:
		for (int i = 0; i < CORNER_COUNT; i++) {
			glm::vec2 c = POOL_CORNER_POS[i];
			glm::vec4 col = i < step ? TEXT_GREEN
				: (i == step ? glm::vec4(1.0f, 0.88f, 0.35f, 0.5f + 0.5f * pulse) : glm::vec4(1, 1, 1, 0.35f));
			drawCircle(c.x, c.y, 0.55f, OUTLINE);
			drawCircle(c.x, c.y, 0.48f, col);
			drawTextCentered(std::to_string(i + 1), c.x, c.y - 0.25f, 0.1f, OUTLINE);
		}
		break;

	case RACE:
		// Flag on the beach
		drawRect(RACE_FLAG.x - 0.05f, RACE_FLAG.y - 1.5f, 0.1f, 1.6f, OUTLINE);
		drawRect(RACE_FLAG.x + 0.05f, RACE_FLAG.y - 1.5f + 0.1f * sinf(time * 8.0f), 0.9f, 0.55f, HEART_BRIGHT);
		// Gabriela running
		drawCharacter(racerPos, npcs[GABRIELA].hair, npcs[GABRIELA].top, npcs[GABRIELA].bottom, npcs[GABRIELA].bitmap);
		break;

	case TREASURE_HUNT:
		// Objects fade in as you get closer (they are "hidden")
		for (int i = 0; i < HIDDEN_COUNT; i++) {
			if (hiddenFound[i]) continue;
			float d = glm::length(HIDDEN_POS[i] - pc);
			float alpha = glm::clamp(1.0f - (d - 1.5f) / (REVEAL_DISTANCE - 1.5f), 0.0f, 1.0f);
			if (alpha <= 0.0f) continue;
			drawCircle(HIDDEN_POS[i].x, HIDDEN_POS[i].y, 0.7f + 0.1f * pulse,
				glm::vec4(1.0f, 1.0f, 0.6f, 0.25f * alpha));
			drawHiddenObject(i, HIDDEN_POS[i], alpha);
		}
		break;

	case CATCH_CRAB:
		drawCrab(crabPos, time);
		break;

	case FOLLOW_TRAIL:
		// Footprints already followed stay faint, only the next one is shown
		for (int i = 0; i < step; i++)
			drawFootprints(TRAIL_POS[i], glm::vec4(0.35f, 0.25f, 0.15f, 0.3f));
		if (step < TRAIL_COUNT) {
			drawCircle(TRAIL_POS[step].x, TRAIL_POS[step].y, 0.6f + 0.15f * pulse,
				glm::vec4(1.0f, 1.0f, 0.6f, 0.3f));
			drawFootprints(TRAIL_POS[step], glm::vec4(0.35f, 0.25f, 0.15f, 1.0f));
		}
		break;
	}
}

void drawCharacters(float time)
{
	for (size_t i = 0; i < npcs.size(); i++) {
		if (!npcVisibleAtSpot((int)i)) continue;
		const Npc& n = npcs[i];
		drawCharacter(n.pos, n.hair, n.top, n.bottom, n.bitmap);
		float cx = n.pos.x + characterSize.x / 2;
		// Name tag under the feet, on a dark panel so it is readable on any ground
		drawLabel(n.name, cx, n.pos.y + characterSize.y + 0.35f, 0.12f, TEXT_YELLOW);

		if (state == EXPLORE) {
			if (n.heartGiven) {
				drawHeart(cx - 0.35f, n.pos.y - 0.9f, 0.1f, HEART_BRIGHT, true);
			}
			else {
				float bob = 0.15f * sinf(time * 4.0f + (float)i);
				drawTextCentered("!", cx, n.pos.y - 1.1f + bob, 0.15f, TEXT_YELLOW);
			}
			if (nearNpc((int)i))
				drawLabel("E - TALK", cx, n.pos.y - 2.2f, 0.12f, TEXT_WHITE);
		}
	}

	// Special dialogue: little hearts float up around the contestant and fade out
	if (state == SPECIAL) {
		const Npc& n = npcs[activeNpc];
		float cx = n.pos.x + characterSize.x / 2;
		for (int k = 0; k < 6; k++) {
			float rise = fmodf(time * 1.5f + k * 0.5f, 3.0f);        // 0 .. 3 squares up
			float x = cx - 0.3f + 1.2f * sinf(time * 2.0f + k * 1.7f);
			float alpha = 1.0f - rise / 3.0f;
			glm::vec4 c = (k % 2 == 0) ? HEART_BRIGHT : HEART_GOLD;
			c.a = alpha;
			drawHeart(x, n.pos.y - 0.5f - rise, 0.09f, c, false);
		}
	}

	// Player
	drawCharacter(playerPos,
		glm::vec4(0.40f, 0.25f, 0.12f, 1.0f),
		glm::vec4(0.95f, 0.35f, 0.60f, 1.0f),
		glm::vec4(0.20f, 0.45f, 0.85f, 1.0f));
}

// Top-left: for each contestant the big heart, one small heart per
// challenge and the relationship score
void drawRelationshipHUD()
{
	float rowH = 1.4f;
	drawRect(0.3f, 0.3f, 16.4f, 0.5f + rowH * npcs.size(), PANEL);

	for (size_t i = 0; i < npcs.size(); i++) {
		const Npc& n = npcs[i];
		float y = 0.6f + i * rowH;

		// Big heart: dark = not earned, red = earned, gold = earned with 100/100
		glm::vec4 bigHeart = !n.heartGiven ? HEART_DARK
			: (n.relationship >= 100 ? HEART_GOLD : HEART_BRIGHT);
		drawHeart(0.6f, y, 0.14f, bigHeart, n.heartGiven);
		drawText(n.name, 1.9f, y + 0.1f, 0.13f, TEXT_WHITE);

		for (int c = 0; c < n.totalChallenges; c++) {
			bool done = c < n.completed;
			drawHeart(6.5f + c * 0.85f, y + 0.15f, 0.09f, done ? HEART_BRIGHT : HEART_DARK, false);
		}

		float barX = 9.6f, barW = 5.0f;   // room for long names like GABRIELA
		drawRect(barX, y + 0.15f, barW, 0.45f, HEART_DARK);
		drawRect(barX, y + 0.15f, barW * n.relationship / 100.0f, 0.45f, PINK);
		drawText(std::to_string(n.relationship), barX + barW + 0.3f, y + 0.1f, 0.13f, TEXT_WHITE);
	}
}

std::string goalText()
{
	switch (activeChallenge) {
	case OBSTACLE_COURSE:
		return "REACH THE FINISH LINE, DODGE THE BEACH BALLS!";
	case POOL_CORNERS:
		return "TOUCH POOL CORNER " + std::to_string(step + 1) + "/4";
	case RACE:
		return "RUN TO THE FLAG ON THE BEACH BEFORE GABRIELA!";
	case TREASURE_HUNT: {
		int found = 0;
		for (int i = 0; i < HIDDEN_COUNT; i++) if (hiddenFound[i]) found++;
		return (found < HIDDEN_COUNT)
			? "FIND BIANCA'S HIDDEN TREASURES  " + std::to_string(found) + "/3"
			: "ALL FOUND! RETURN TO BIANCA";
	}
	case CATCH_CRAB:
		return "CATCH THE RUNAWAY CRAB!";
	case FOLLOW_TRAIL:
		return "FOLLOW THE FOOTPRINTS  " + std::to_string(step) + "/" + std::to_string(TRAIL_COUNT);
	}
	return "";
}

void drawChallengeHUD()
{
	if (state != CHALLENGE) return;

	// Timer panel in the top-right corner
	float px = 0.16f;
	float limit = CHALLENGES[activeChallenge].timeLimit;
	int seconds = (int)ceilf(glm::max(timeLeft, 0.0f));
	glm::vec4 timeColor = timeLeft < 5.0f ? TEXT_RED : TEXT_WHITE;
	drawRect(32.0f, 0.3f, 7.7f, 1.9f, PANEL);
	drawText("TIME: " + std::to_string(seconds), 32.4f, 0.55f, px, timeColor);
	drawRect(32.4f, 1.6f, 6.9f, 0.3f, HEART_DARK);
	drawRect(32.4f, 1.6f, 6.9f * (timeLeft / limit), 0.3f, timeColor);

	drawLabel(goalText(), MAP_W / 2, 26.5f, 0.13f, TEXT_YELLOW);
}

void drawDialogBox()
{
	if (state != TALK && state != OFFER && state != SPECIAL) return;
	const Npc& n = npcs[activeNpc];
	float px = 0.15f;

	// The special dialogue gets a golden frame
	drawRect(3.0f, 19.0f, 34.0f, 6.5f, state == SPECIAL ? HEART_GOLD : OUTLINE);
	drawRect(3.15f, 19.15f, 33.7f, 6.2f, glm::vec4(0.15f, 0.08f, 0.12f, 0.95f));
	drawText(n.name + ":", 4.0f, 19.8f, px, TEXT_YELLOW);

	if (state == SPECIAL) {
		std::string title = "PERFECT MATCH 100/100";
		float tx = 36.0f - textWidth(title, 0.12f);
		drawHeart(tx - 1.2f, 19.75f, 0.12f, HEART_GOLD, true);
		drawText(title, tx, 19.85f, 0.12f, HEART_GOLD);
		drawText(n.special1, 4.0f, 21.0f, 0.13f, TEXT_WHITE);
		drawText(n.special2, 4.0f, 21.9f, 0.13f, TEXT_WHITE);
		drawText(n.specialAnswer1, 4.0f, 23.0f, px, PINK);
		drawText(n.specialAnswer2, 4.0f, 24.1f, px, TEXT_GREEN);
	}
	else if (state == TALK) {
		drawText(n.greeting, 4.0f, 21.0f, px, TEXT_WHITE);
		drawText(n.answer1, 4.0f, 22.6f, px, TEXT_GREEN);
		drawText(n.answer2, 4.0f, 23.8f, px, PINK);
	}
	else {
		std::string counter = "CHALLENGE " + std::to_string(n.completed + 1) + "/" + std::to_string(n.totalChallenges);
		drawText(counter, 36.0f - textWidth(counter, 0.12f), 19.85f, 0.12f, PINK);
		drawText(CHALLENGES[n.queue.front()].offer, 4.0f, 21.0f, 0.13f, TEXT_WHITE);
		drawText("WIN TO GET CLOSER TO ME. IF YOU FAIL, YOU LOSE A HEART.", 4.0f, 22.2f, 0.12f, TEXT_WHITE);
		drawText("Y - ACCEPT", 4.0f, 23.8f, px, TEXT_GREEN);
		drawText("N - REFUSE", 13.0f, 23.8f, px, TEXT_RED);
	}
}

void drawMessage()
{
	if (messageTimer > 0.0f && state != GAME_OVER && state != TEASER_END)
		drawLabel(message, MAP_W / 2, 3.0f, 0.17f, TEXT_WHITE);
}

void drawEndScreen()
{
	if (state != GAME_OVER && state != TEASER_END) return;
	drawRect(0, 0, MAP_W, MAP_H, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

	if (state == GAME_OVER) {
		drawTextCentered("GAME OVER", MAP_W / 2, 9.0f, 0.6f, TEXT_RED);
		drawTextCentered("YOU HAVE BEEN DUMPED FROM THE ISLAND", MAP_W / 2, 14.0f, 0.18f, TEXT_WHITE);
	}
	else {
		drawTextCentered("BOTH HEARTS COLLECTED!", MAP_W / 2, 8.0f, 0.35f, HEART_BRIGHT);
		drawTextCentered("THE ADVENTURE CONTINUES INSIDE THE VILLA...", MAP_W / 2, 11.5f, 0.18f, TEXT_WHITE);
		for (size_t i = 0; i < npcs.size(); i++) {
			const Npc& n = npcs[i];
			std::string line = n.name + ": " + std::to_string(n.relationship) + "/100";
			glm::vec4 color = PINK;
			if (n.specialChoice == 1) { line += " - YOUR PARTNER"; color = HEART_GOLD; }
			else if (n.specialChoice == 2) { line += " - BEST FRIEND";  color = HEART_GOLD; }
			drawTextCentered(line, MAP_W / 2, 14.0f + i * 1.2f, 0.18f, color);
		}
	}
	drawTextCentered("PRESS R TO PLAY AGAIN", MAP_W / 2, 18.0f, 0.15f, TEXT_YELLOW);
}
