#pragma once

// the 6 challenges and their data

#include <vector>
#include <string>
#include <cmath>
#include "glm/glm.hpp"

#include "World.hpp"

// Challenges
enum ChallengeType {
	OBSTACLE_COURSE,   // Gabriela: dodge beach balls, reach the finish line
	POOL_CORNERS,      // Gabriela: touch the 4 pool corners in order
	RACE,              // Gabriela: race Gabriela to the flag on the beach
	TREASURE_HUNT,     // Bianca: find 3 hidden objects, bring them back
	CATCH_CRAB,        // Bianca: catch a crab that runs away from you
	FOLLOW_TRAIL,      // Bianca: follow footprints that appear one by one
	CHALLENGE_COUNT	   // Total number of challenges
};

struct Challenge {
	std::string offer;     // Challenge description
	float timeLimit;       // Time available in seconds
};

Challenge CHALLENGES[CHALLENGE_COUNT]; // Array for all challenges

// Set the description and time limit for each challenge
void initChallenges()
{
	// Gabriela: dodge beach balls, reach the finish line
	CHALLENGES[OBSTACLE_COURSE].offer = "BEAT MY POOLSIDE OBSTACLE COURSE IN 20 SECONDS!";
	CHALLENGES[OBSTACLE_COURSE].timeLimit = 20.0f;

	// Gabriela: touch the 4 pool corners in order
	CHALLENGES[POOL_CORNERS].offer = "TOUCH ALL 4 CORNERS OF THE POOL IN ORDER IN 15 SECONDS!";
	CHALLENGES[POOL_CORNERS].timeLimit = 15.0f;

	// Gabriela: race Gabriela to the flag on the beach
	CHALLENGES[RACE].offer = "RACE ME TO THE FLAG ON THE BEACH. LOSER BUYS THE DRINKS!";
	CHALLENGES[RACE].timeLimit = 12.0f;

	// Bianca: find 3 hidden objects, bring them back
	CHALLENGES[TREASURE_HUNT].offer = "FIND MY 3 HIDDEN TREASURES AND BRING THEM BACK IN 40 SECONDS!";
	CHALLENGES[TREASURE_HUNT].timeLimit = 40.0f;

	// Bianca: catch a crab that runs away from you
	CHALLENGES[CATCH_CRAB].offer = "MY PET CRAB RAN AWAY! CATCH HIM IN 25 SECONDS!";
	CHALLENGES[CATCH_CRAB].timeLimit = 25.0f;

	// Bianca: follow footprints that appear one by one
	CHALLENGES[FOLLOW_TRAIL].offer = "FOLLOW MY SECRET TRAIL OF FOOTPRINTS IN 35 SECONDS!";
	CHALLENGES[FOLLOW_TRAIL].timeLimit = 35.0f;
}


// Obstacle course settings
const Box COURSE(12.0f, 20.0f, 19.5f, 5.5f);       // Area where the obstacle course takes place
const glm::vec2 COURSE_START(12.3f, 22.1f);		   // Player starting position
const Box COURSE_FINISH(29.5f, 20.0f, 2.0f, 5.5f); // Finish area
const int BALL_COUNT = 5;						   // Nr of beach balls
const float BALL_RADIUS = 0.6f;					   // Size of beach balls

// Calculate the position of each moving ball
glm::vec2 ballPosition(int i, float time)
{
	float x = 15.0f + i * 3.2f;								 // Set the horizontal position
	float speed = 1.6f + 0.35f * i;							 // Each ball moves at a different speed
	float y = 22.75f + 2.1f * sinf(time * speed + i * 1.3f); // Move the ball up and down using sin
	return glm::vec2(x, y);									 // Return the new position
}

// Pool corners challenge
const int CORNER_COUNT = 4;							// Number of corners

// Positions of the corners in the correct order
const glm::vec2 POOL_CORNER_POS[CORNER_COUNT] = {
	glm::vec2(14.0f, 13.3f), glm::vec2(26.0f, 13.3f),
	glm::vec2(26.0f, 19.8f), glm::vec2(14.0f, 19.8f)
};

// Race: Gabriela runs along these points (top-left of the runner)
const glm::vec2 PLAYER_NEXT_TO_MARCO(28.2f, 12.3f); // Player position before the race

// Path used by Gabriela during the race
const int RACE_POINTS = 2;
const glm::vec2 RACE_PATH[RACE_POINTS] = { glm::vec2(27.0f, 24.5f), glm::vec2(19.55f, 26.0f) };
const glm::vec2 RACE_FLAG(20.0f, 26.6f);		   // Position of the finish flag
const float RACER_SPEED = 4.5f;			           // Gabriela's running speed

// Treasure hunt
const int HIDDEN_COUNT = 3;		// Number of hidden objects
const glm::vec2 HIDDEN_POS[HIDDEN_COUNT] = {
	glm::vec2(4.5f, 5.5f),      // shell near the gate
	glm::vec2(38.5f, 5.5f),     // sunglasses behind the hideaway
	glm::vec2(3.0f, 24.8f)      // flower near the beach
};
const std::string HIDDEN_NAMES[HIDDEN_COUNT] = { "SHELL", "SUNGLASSES", "FLOWER" };
const float REVEAL_DISTANCE = 4.0f;   // objects become visible when you get close

// Runaway crab
const glm::vec2 CRAB_START(8.0f, 21.5f); // Starting position of the crab
const float CRAB_FLEE_SPEED = 4.0f;      // Speed when the crab runs away
const float CRAB_WANDER_SPEED = 2.0f;    // Speed when the crab moves normally
const float CRAB_SCARE_DISTANCE = 5.0f;  // Distance at which the crab gets scared

// Footprint trail
const int TRAIL_COUNT = 6;				// Number of footprints
// Positions of the footprints
const glm::vec2 TRAIL_POS[TRAIL_COUNT] = {
	glm::vec2(9.5f, 19.5f), glm::vec2(5.0f, 21.5f), glm::vec2(3.5f, 13.0f),
	glm::vec2(9.5f, 9.0f), glm::vec2(29.5f, 11.0f), glm::vec2(38.5f, 15.5f)
};
