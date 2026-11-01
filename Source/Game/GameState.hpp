#pragma once
// GameState.hpp
// Everything that changes while playing

#include <vector>
#include <string>
#include <cmath>
#include "glm/glm.hpp"

#include "Config.hpp"
#include "Challenges.hpp"

// Player position (top-left corner of the player box)
glm::vec2 playerPos = PLAYER_START;

// Game state
enum GameState { EXPLORE, TALK, OFFER, CHALLENGE, SPECIAL, GAME_OVER, TEASER_END };
GameState state = EXPLORE;

int activeNpc = -1;          // contestant we are talking to / playing for
int activeChallenge = -1;    // ChallengeType being played
float timeLeft = 0.0f;

// Progress of the running challenge
int step = 0;                        // pool corner / footprint index
bool hiddenFound[HIDDEN_COUNT];
glm::vec2 racerPos;
int racerTarget = 0;
glm::vec2 crabPos;
glm::vec2 crabDir(1.0f, 0.0f);

// Every finished challenge is one small heart: (contestant, challenge)
struct EarnedHeart {
	int npc, challenge;
	EarnedHeart(int n, int c) : npc(n), challenge(c) {}
};
std::vector<EarnedHeart> earnedHearts;

std::string message;         // short feedback text at the top
float messageTimer = 0.0f;
