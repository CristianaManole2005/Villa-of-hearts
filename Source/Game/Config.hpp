#pragma once

// map size, player settings

#include "glm/glm.hpp"

// The map is 40 x 30 squares, (0,0) is top-left, y grows downward
const float MAP_W = 40.0f;
const float MAP_H = 30.0f;

const glm::vec2 PLAYER_START(2.0f, 8.0f);	 // Player starting position
const glm::vec2 characterSize(0.9f, 1.2f);   // Size of the player and NPCs
const float playerSpeed = 6.0f;              // Player movement speed (squares per second)
const float TALK_DISTANCE = 2.0f;			 // Maximum distance for talking to NPCs
