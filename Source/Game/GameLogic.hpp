#pragma once
// ============================================================
// GameLogic.hpp
// Collisions, dialogue, challenge rules and movement
// ============================================================

#include <vector>
#include <string>
#include <cmath>
#include "glew/glew.h"   // GLEW must come before GLFW
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

#include "Config.hpp"
#include "World.hpp"
#include "Challenges.hpp"
#include "Npcs.hpp"
#include "GameState.hpp"
#include "Input.hpp"

// Collision with the world and the contestants
// Gabriela is not standing at her spot while she is racing
bool npcVisibleAtSpot(int i)
{
	return !(state == CHALLENGE && activeChallenge == RACE && i == GABRIELA);
}

bool collides(glm::vec2 p)
{
	// During the obstacle course the player must stay on the course
	if (state == CHALLENGE && activeChallenge == OBSTACLE_COURSE)
		return !rectInside(p, characterSize, COURSE);

	// Map borders (the sea starts at y = 28)
	if (p.x < 0 || p.y < 0 || p.x + characterSize.x > MAP_W || p.y + characterSize.y > 28.0f)
		return true;
	for (const Box& b : solids)
		if (rectsOverlap(p, characterSize, b)) return true;
	for (size_t i = 0; i < npcs.size(); i++)
		if (npcVisibleAtSpot((int)i) &&
			rectsOverlap(p, characterSize, Box(npcs[i].pos.x, npcs[i].pos.y, characterSize.x, characterSize.y)))
			return true;
	if (circleRectOverlap(firePitCenter, firePitRadius, p, characterSize)) return true;
	for (const Tree& t : trees)
		if (circleRectOverlap(t.pos, 0.35f, p, characterSize)) return true;
	return false;
}

// Is a single point blocked? (used for the crab, with a small margin)
bool pointBlocked(glm::vec2 c)
{
	const float m = 0.4f;
	if (c.x < m || c.y < m || c.x > MAP_W - m || c.y > 28.0f - m) return true;
	for (const Box& b : solids)
		if (c.x > b.x - m && c.x < b.x + b.w + m && c.y > b.y - m && c.y < b.y + b.h + m)
			return true;
	if (glm::length(c - firePitCenter) < firePitRadius + m) return true;
	return false;
}

glm::vec2 playerCenter() { return playerPos + characterSize * 0.5f; }

bool nearNpc(int i)
{
	glm::vec2 npcCenter = npcs[i].pos + characterSize * 0.5f;
	return glm::length(playerCenter() - npcCenter) < TALK_DISTANCE;
}

int closestNpcInRange()
{
	for (size_t i = 0; i < npcs.size(); i++)
		if (nearNpc((int)i)) return (int)i;
	return -1;
}

// Dialogue and challenge logic
void showMessage(const std::string& text)
{
	message = text;
	messageTimer = 3.0f;
}

bool allHeartsCollected()
{
	for (const Npc& n : npcs)
		if (!n.heartGiven) return false;
	return true;
}

void startTalking(int npc)
{
	Npc& n = npcs[npc];
	if (n.heartGiven) {
		if (n.specialChoice == 1)      showMessage(n.specialReaction1);
		else if (n.specialChoice == 2) showMessage(n.specialReaction2);
		else if (n.relationship < 100) showMessage(n.almostThere);
		else                           showMessage(n.thanks);
		return;
	}
	activeNpc = npc;
	// First meeting: answer choices. Later: straight to the next challenge.
	state = n.talkedOnce ? OFFER : TALK;
}

void chooseAnswer(int answer)
{
	Npc& n = npcs[activeNpc];
	if (answer == 1) {
		changeRelationship(activeNpc, n.answer1Effect);
		showMessage(n.reaction1);
	}
	else {
		changeRelationship(activeNpc, n.answer2Effect);
		showMessage(n.reaction2);
	}
	n.talkedOnce = true;
	state = OFFER;
}

void startChallenge()
{
	activeChallenge = npcs[activeNpc].queue.front();
	timeLeft = CHALLENGES[activeChallenge].timeLimit;
	step = 0;

	switch (activeChallenge) {
	case OBSTACLE_COURSE:
		playerPos = COURSE_START;               // teleport to the start line
		break;
	case RACE:
		playerPos = PLAYER_NEXT_TO_ALEX;        // both start at the same place
		racerPos = npcs[GABRIELA].pos;
		racerTarget = 0;
		break;
	case TREASURE_HUNT:
		for (int i = 0; i < HIDDEN_COUNT; i++) hiddenFound[i] = false;
		break;
	case CATCH_CRAB:
		crabPos = CRAB_START;
		crabDir = glm::vec2(1.0f, 0.0f);
		break;
	default:
		break;
	}
	state = CHALLENGE;
}

void refuseChallenge()
{
	Npc& n = npcs[activeNpc];
	changeRelationship(activeNpc, -5);
	rotateQueue(n);                             // next time: a different challenge
	showMessage(n.name + " IS DISAPPOINTED. -5");
	activeNpc = -1;
	state = EXPLORE;
}

// After challenges that move the player, put them back near the contestant
void returnPlayerAfterChallenge()
{
	if (activeChallenge == OBSTACLE_COURSE || activeChallenge == RACE)
		playerPos = PLAYER_NEXT_TO_ALEX;
}

void completeChallenge()
{
	Npc& n = npcs[activeNpc];
	int reward = challengeReward(timeLeft, CHALLENGES[activeChallenge].timeLimit);
	changeRelationship(activeNpc, reward);

	// Remove it from the queue: it will never be offered again
	n.queue.erase(n.queue.begin());
	n.completed++;
	earnedHearts.push_back(EarnedHeart(activeNpc, activeChallenge));

	returnPlayerAfterChallenge();
	activeChallenge = -1;

	if (n.completed == n.totalChallenges) {
		n.heartGiven = true;

		// Full relationship -> special dialogue (only once)
		if (n.relationship >= 100 && n.specialChoice == 0) {
			showMessage("100/100! " + n.name + " WANTS TO TELL YOU SOMETHING...");
			state = SPECIAL;                    // activeNpc stays set for the dialogue
			return;
		}
		showMessage(n.name + "'S HEART COLLECTED! +" + std::to_string(reward) +
			"  (" + std::to_string(n.relationship) + "/100)");
	}
	else {
		showMessage("CHALLENGE COMPLETE! +" + std::to_string(reward) + "  (" +
			std::to_string(n.completed) + "/" + std::to_string(n.totalChallenges) + ")");
	}

	activeNpc = -1;
	state = allHeartsCollected() ? TEASER_END : EXPLORE;
}

// Answer in the special dialogue: 1 = couple up, 2 = stay friends
void chooseSpecialAnswer(int answer)
{
	Npc& n = npcs[activeNpc];
	n.specialChoice = answer;
	showMessage(answer == 1 ? n.specialReaction1 : n.specialReaction2);
	activeNpc = -1;
	state = allHeartsCollected() ? TEASER_END : EXPLORE;
}

void failChallenge(const std::string& reason)
{
	Npc& n = npcs[activeNpc];
	changeRelationship(activeNpc, -10);
	rotateQueue(n);                             // next time: a different challenge
	returnPlayerAfterChallenge();

	if (earnedHearts.empty()) {
		state = GAME_OVER;
	}
	else {
		// Lose the most recent small heart; that challenge must be done again
		EarnedHeart lost = earnedHearts.back();
		earnedHearts.pop_back();
		Npc& owner = npcs[lost.npc];
		owner.completed--;
		owner.heartGiven = false;
		owner.queue.push_back(lost.challenge);
		showMessage(reason + " YOU LOST A HEART");
		state = EXPLORE;
	}
	activeNpc = -1;
	activeChallenge = -1;
}

// Moves the crab: runs away from the player when close, wanders otherwise,
// and turns 90 degrees when it would hit something
void updateCrab(float deltaTime, float time)
{
	glm::vec2 away = crabPos - playerCenter();
	float dist = glm::length(away);
	glm::vec2 desired;
	float speed;
	if (dist < CRAB_SCARE_DISTANCE && dist > 0.001f) {
		desired = away / dist;
		speed = CRAB_FLEE_SPEED;
	}
	else {
		desired = glm::normalize(glm::vec2(cosf(time * 0.7f), sinf(time * 1.1f)));
		speed = CRAB_WANDER_SPEED;
	}

	glm::vec2 options[3] = { desired, glm::vec2(desired.y, -desired.x), glm::vec2(-desired.y, desired.x) };
	for (int i = 0; i < 3; i++) {
		glm::vec2 next = crabPos + options[i] * speed * deltaTime;
		if (!pointBlocked(next)) {
			crabPos = next;
			crabDir = options[i];
			break;
		}
	}
}

// Moves Gabriela along the race path at a constant speed
// Returns true when Gabriela reaches the flag
bool updateRacer(float deltaTime)
{
	glm::vec2 target = RACE_PATH[racerTarget];
	glm::vec2 toTarget = target - racerPos;
	float dist = glm::length(toTarget);
	float move = RACER_SPEED * deltaTime;
	if (dist <= move) {
		racerPos = target;
		racerTarget++;
		return racerTarget >= RACE_POINTS;
	}
	racerPos += toTarget / dist * move;
	return false;
}

void updateChallenge(float deltaTime, float time)
{
	glm::vec2 pc = playerCenter();

	switch (activeChallenge) {
	case OBSTACLE_COURSE:
		for (int i = 0; i < BALL_COUNT; i++)
			if (circleRectOverlap(ballPosition(i, time), BALL_RADIUS, playerPos, characterSize)) {
				playerPos = COURSE_START;
				showMessage("OUCH! BACK TO THE START");
				break;
			}
		if (rectsOverlap(playerPos, characterSize, COURSE_FINISH)) {
			completeChallenge();
			return;
		}
		break;

	case POOL_CORNERS:
		if (glm::length(POOL_CORNER_POS[step] - pc) < 0.9f) {
			step++;
			if (step == CORNER_COUNT) {
				completeChallenge();
				return;
			}
		}
		break;

	case RACE:
		if (glm::length(RACE_FLAG - pc) < 1.0f) {
			completeChallenge();
			return;
		}
		if (updateRacer(deltaTime)) {
			failChallenge("GABRIELA WON THE RACE!");
			return;
		}
		break;

	case TREASURE_HUNT: {
		int found = 0;
		for (int i = 0; i < HIDDEN_COUNT; i++) {
			if (!hiddenFound[i] && glm::length(HIDDEN_POS[i] - pc) < 0.9f) {
				hiddenFound[i] = true;
				showMessage("YOU FOUND THE " + HIDDEN_NAMES[i] + "!");
			}
			if (hiddenFound[i]) found++;
		}
		if (found == HIDDEN_COUNT && nearNpc(BIANCA)) {
			completeChallenge();
			return;
		}
		break;
	}

	case CATCH_CRAB:
		updateCrab(deltaTime, time);
		if (glm::length(crabPos - pc) < 0.9f) {
			completeChallenge();
			return;
		}
		break;

	case FOLLOW_TRAIL:
		if (glm::length(TRAIL_POS[step] - pc) < 0.9f) {
			step++;
			if (step == TRAIL_COUNT) {
				completeChallenge();
				return;
			}
		}
		break;
	}

	timeLeft -= deltaTime;
	if (timeLeft <= 0.0f)
		failChallenge("TIME IS UP!");
}

void restartGame()
{
	initNpcs();
	earnedHearts.clear();
	playerPos = PLAYER_START;
	activeNpc = -1;
	activeChallenge = -1;
	state = EXPLORE;
	showMessage("WELCOME TO LOVE ISLAND! TALK TO GABRIELA AND BIANCA");
}

// Movement and update

void movePlayer(GLFWwindow* window, float deltaTime)
{
	glm::vec2 dir(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir.y -= 1.0f;  // y grows down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir.y += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir.x -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir.x += 1.0f;
	if (glm::length(dir) > 0.0f) dir = glm::normalize(dir);

	glm::vec2 step = dir * playerSpeed * deltaTime;

	// Move one axis at a time so the player slides along walls
	glm::vec2 tryX(playerPos.x + step.x, playerPos.y);
	if (!collides(tryX)) playerPos = tryX;
	glm::vec2 tryY(playerPos.x, playerPos.y + step.y);
	if (!collides(tryY)) playerPos = tryY;
}

void updateGame(GLFWwindow* window, float deltaTime, float time)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	updateKeys(window);
	if (messageTimer > 0.0f) messageTimer -= deltaTime;

	switch (state) {
	case GAME_OVER:
	case TEASER_END:
		if (keyPressed(GLFW_KEY_R)) restartGame();
		break;

	case SPECIAL:
		if (keyPressed(GLFW_KEY_1)) chooseSpecialAnswer(1);
		else if (keyPressed(GLFW_KEY_2)) chooseSpecialAnswer(2);
		break;

	case TALK:
		if (keyPressed(GLFW_KEY_1)) chooseAnswer(1);
		else if (keyPressed(GLFW_KEY_2)) chooseAnswer(2);
		break;

	case OFFER:
		if (keyPressed(GLFW_KEY_Y)) startChallenge();
		else if (keyPressed(GLFW_KEY_N)) refuseChallenge();
		break;

	case EXPLORE:
		movePlayer(window, deltaTime);
		if (keyPressed(GLFW_KEY_E)) {
			int npc = closestNpcInRange();
			if (npc >= 0) startTalking(npc);
		}
		break;

	case CHALLENGE:
		movePlayer(window, deltaTime);
		updateChallenge(deltaTime, time);
		break;
	}
}
