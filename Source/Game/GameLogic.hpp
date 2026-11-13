#pragma once

// collisions, dialogue, challenge rules, movement

#include <vector>
#include <string>
#include <cmath>
#include "glew/glew.h"  
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

#include "Config.hpp"
#include "World.hpp"
#include "Challenges.hpp"
#include "Npcs.hpp"
#include "GameState.hpp"
#include "Input.hpp"

// Hide Gabriela from her usual position during the race
bool npcVisibleAtSpot(int i)
{
	return !(state == CHALLENGE && activeChallenge == RACE && i == GABRIELA);
}

// Check if the player hits an obstacle
bool collides(glm::vec2 p)
{
	// During the obstacle course the player must stay on the course
	if (state == CHALLENGE && activeChallenge == OBSTACLE_COURSE)
		return !rectInside(p, characterSize, COURSE);

	// Map borders (the sea starts at y = 28)
	if (p.x < 0 || p.y < 0 || p.x + characterSize.x > MAP_W || p.y + characterSize.y > 28.0f)
		return true;

	// Check buildings and other objects
	for (const Box& b : solids)
		if (rectsOverlap(p, characterSize, b)) return true;

	// Check collisions with NPCs
	for (size_t i = 0; i < npcs.size(); i++)
		if (npcVisibleAtSpot((int)i) &&
			rectsOverlap(p, characterSize, Box(npcs[i].pos.x, npcs[i].pos.y, characterSize.x, characterSize.y)))
			return true;

	// Check the fire pit and trees
	if (circleRectOverlap(firePitCenter, firePitRadius, p, characterSize)) return true;
	for (const Tree& t : trees)
		if (circleRectOverlap(t.pos, 0.35f, p, characterSize)) return true;
	return false;
}

// Check if a position is blocked for the crab
bool pointBlocked(glm::vec2 c)
{
	const float m = 0.4f;
	if (c.x < m || c.y < m || c.x > MAP_W - m || c.y > 28.0f - m) return true; // Check the map limits

	// Check the buildings
	for (const Box& b : solids)
		if (c.x > b.x - m && c.x < b.x + b.w + m && c.y > b.y - m && c.y < b.y + b.h + m)
			return true;

	// Check the fire pit
	if (glm::length(c - firePitCenter) < firePitRadius + m) return true;
	return false;
}

glm::vec2 playerCenter() { return playerPos + characterSize * 0.5f; } // Calculate the center of the player

// Check if the player is close enough to talk
bool nearNpc(int i)
{
	glm::vec2 npcCenter = npcs[i].pos + characterSize * 0.5f;
	return glm::length(playerCenter() - npcCenter) < TALK_DISTANCE;
}

// Find an NPC near the player
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

// Check if all NPC hearts are collected
bool allHeartsCollected()
{
	for (const Npc& n : npcs)
		if (!n.heartGiven) return false;
	return true;
}

// Start a conversation with an NPC
void startTalking(int npc)
{
	Npc& n = npcs[npc];

	// Show the correct message if the heart was already earned
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

// Handle the player's dialogue answer
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

// Set up the selected challenge
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
		playerPos = PLAYER_NEXT_TO_MARCO;        // both start at the same place
		racerPos = npcs[GABRIELA].pos;
		racerTarget = 0;
		break;
	case TREASURE_HUNT:
		for (int i = 0; i < HIDDEN_COUNT; i++) hiddenFound[i] = false; // Reset the hidden objects
		break;
	case CATCH_CRAB:
		// Reset the crab position and direction
		crabPos = CRAB_START;
		crabDir = glm::vec2(1.0f, 0.0f);
		break;
	default:
		break;
	}
	state = CHALLENGE;
}

// Handle a refused challenge
void refuseChallenge()
{
	Npc& n = npcs[activeNpc];
	changeRelationship(activeNpc, -5);			// Lose 5 relationship points
	rotateQueue(n);                             // next time: a different challenge
	showMessage(n.name + " IS DISAPPOINTED. -5");
	activeNpc = -1;
	state = EXPLORE;
}

// After challenges that move the player, put them back near the contestant
void returnPlayerAfterChallenge()
{
	if (activeChallenge == OBSTACLE_COURSE || activeChallenge == RACE)
		playerPos = PLAYER_NEXT_TO_MARCO;
}

// Give rewards and update challenge progress
void completeChallenge()
{
	Npc& n = npcs[activeNpc];

	// Calculate the relationship reward
	int reward = challengeReward(timeLeft, CHALLENGES[activeChallenge].timeLimit);
	changeRelationship(activeNpc, reward);

	// Remove it from the queue: it will never be offered again
	n.queue.erase(n.queue.begin());
	n.completed++;
	earnedHearts.push_back(EarnedHeart(activeNpc, activeChallenge));	// Add a new heart

	returnPlayerAfterChallenge();
	activeChallenge = -1;

	// Check if all challenges for this NPC are completed
	if (n.completed == n.totalChallenges) {
		n.heartGiven = true;

		// Full relationship -> special dialogue (only once)
		if (n.relationship >= 100 && n.specialChoice == 0) {
			showMessage("100/100! " + n.name + " WANTS TO TELL YOU SOMETHING...");
			state = SPECIAL;
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

// Handle a failed challenge
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

		// Add the lost challenge back to its NPC
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

//Crab movement
void updateCrab(float deltaTime, float time)
{
	// Calculate the distance from the player
	glm::vec2 away = crabPos - playerCenter();
	float dist = glm::length(away);
	glm::vec2 desired;
	float speed;

	// Run away when the player gets close
	if (dist < CRAB_SCARE_DISTANCE && dist > 0.001f) {
		desired = away / dist;
		speed = CRAB_FLEE_SPEED;
	}
	else {
		// Move around when the player is far away
		desired = glm::normalize(glm::vec2(cosf(time * 0.7f), sinf(time * 1.1f)));
		speed = CRAB_WANDER_SPEED;
	}

	glm::vec2 options[3] = { desired, glm::vec2(desired.y, -desired.x), glm::vec2(-desired.y, desired.x) }; // Try different directions to avoid obstacles
	for (int i = 0; i < 3; i++) {
		glm::vec2 next = crabPos + options[i] * speed * deltaTime;

		// Move if the new position is free
		if (!pointBlocked(next)) {
			crabPos = next;
			crabDir = options[i];
			break;
		}
	}
}

// Move Gabriela during the race
bool updateRacer(float deltaTime)
{
	// Get the next target position
	glm::vec2 target = RACE_PATH[racerTarget];
	glm::vec2 toTarget = target - racerPos;
	float dist = glm::length(toTarget);
	float move = RACER_SPEED * deltaTime;

	// Check if Gabriela reached the target
	if (dist <= move) {
		racerPos = target;
		racerTarget++;
		return racerTarget >= RACE_POINTS;
	}

	// Move towards the target
	racerPos += toTarget / dist * move;
	return false;
}

// Update the current challenge
void updateChallenge(float deltaTime, float time)
{
	glm::vec2 pc = playerCenter();

	switch (activeChallenge) {
	case OBSTACLE_COURSE:
		// Check collisions with the moving balls
		for (int i = 0; i < BALL_COUNT; i++)
			if (circleRectOverlap(ballPosition(i, time), BALL_RADIUS, playerPos, characterSize)) {
				playerPos = COURSE_START;
				showMessage("OUCH! BACK TO THE START");
				break;
			}

		// Check if the player reached the finish
		if (rectsOverlap(playerPos, characterSize, COURSE_FINISH)) {
			completeChallenge();
			return;
		}
		break;

	case POOL_CORNERS:
		// Check if the player reached the next corner
		if (glm::length(POOL_CORNER_POS[step] - pc) < 0.9f) {
			step++;

			// Complete the challenge after all 4 corners
			if (step == CORNER_COUNT) {
				completeChallenge();
				return;
			}
		}
		break;

	case RACE:
		// Check if the player reached the flag
		if (glm::length(RACE_FLAG - pc) < 1.0f) {
			completeChallenge();
			return;
		}

		// Check if Gabriela won
		if (updateRacer(deltaTime)) {
			failChallenge("GABRIELA WON THE RACE!");
			return;
		}
		break;

	case TREASURE_HUNT: {
		int found = 0;

		// Check each hidden object
		for (int i = 0; i < HIDDEN_COUNT; i++) {
			if (!hiddenFound[i] && glm::length(HIDDEN_POS[i] - pc) < 0.9f) {
				hiddenFound[i] = true;
				showMessage("YOU FOUND THE " + HIDDEN_NAMES[i] + "!");
			}
			if (hiddenFound[i]) found++;
		}

		// Return to Bianca after finding everything
		if (found == HIDDEN_COUNT && nearNpc(BIANCA)) {
			completeChallenge();
			return;
		}
		break;
	}

	case CATCH_CRAB:
		// Move the crab
		updateCrab(deltaTime, time);

		// Check if the player caught it
		if (glm::length(crabPos - pc) < 0.9f) {
			completeChallenge();
			return;
		}
		break;

	case FOLLOW_TRAIL:
		// Check if the player reached the next footprint
		if (glm::length(TRAIL_POS[step] - pc) < 0.9f) {
			step++;

			// Complete the challenge after all footprints
			if (step == TRAIL_COUNT) {
				completeChallenge();
				return;
			}
		}
		break;
	}

	timeLeft -= deltaTime;				// Update the challenge timer

	// Fail when the time runs out
	if (timeLeft <= 0.0f)
		failChallenge("TIME IS UP!");
}

// Reset the game
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


// Move the player using WASD
void movePlayer(GLFWwindow* window, float deltaTime)
{
	glm::vec2 dir(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir.y -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir.y += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir.x -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir.x += 1.0f;
	if (glm::length(dir) > 0.0f) dir = glm::normalize(dir);			// Normalize the direction for diagonal movement

	glm::vec2 step = dir * playerSpeed * deltaTime;					// Calculate movement using deltaTime

	// Check horizontal movement
	glm::vec2 tryX(playerPos.x + step.x, playerPos.y);
	if (!collides(tryX)) playerPos = tryX;

	// Check vertical movement
	glm::vec2 tryY(playerPos.x, playerPos.y + step.y);
	if (!collides(tryY)) playerPos = tryY;
}

// Update the game based on its current state
void updateGame(GLFWwindow* window, float deltaTime, float time)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Update keyboard input and message timer
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
