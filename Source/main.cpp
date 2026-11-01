#pragma once
// ============================================================
// Love Island: Win Their Hearts - Part 1 (2D teaser)
//
// The player arrives at the villa garden and meets two contestants:
//  - Gabriela (the competitive one): obstacle course, pool corners, race
//  - Bianca  (the adventurous one): hidden treasures, runaway crab, footprint trail
// Each contestant offers 3 different challenges, one at a time, and never
// offers a finished challenge again. Every finished challenge gives a small
// heart; finishing all 3 of a contestant gives their big heart.
// Each contestant has a relationship score (0-100) that changes with
// dialogue choices and challenge results.
// Collecting both big hearts ends the teaser.
// ============================================================

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW and GLFW
#include "glew/glew.h"
#include "glfw/glfw3.h"

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Include our helper function for loading shaders
#include "Shaders/Utils/shaderLoader.hpp"

// Global variables
#include "Globals/globals.hpp"

// Game files (see the Game folder)
#include "Game/Config.hpp"       // map size, player settings
#include "Game/Colors.hpp"       // all colors
#include "Game/PixelArt.hpp"     // heart and character bitmaps
#include "Game/Renderer.hpp"     // OpenGL setup, drawRect, drawCircle, drawHeart...
#include "Game/TextRenderer.hpp" // text with the Press Start 2P font (stb_truetype)
#include "Game/World.hpp"        // buildings, trees, fire pit, overlap tests
#include "Game/Challenges.hpp"   // the 6 challenges and their data
#include "Game/Npcs.hpp"         // Gabriela and Bianca
#include "Game/GameState.hpp"    // everything that changes while playing
#include "Game/Input.hpp"        // keyboard helper
#include "Game/GameLogic.hpp"    // collisions, dialogue, challenge rules, movement
#include "Game/GameDraw.hpp"     // drawing the map, challenges, characters, UI

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

	#ifdef __APPLE__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	window = glfwCreateWindow(windowWidth, windowHeight, "Love Island: Win Their Hearts", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	int fbw, fbh;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	setViewport(fbw, fbh);
	glfwSetFramebufferSizeCallback(window, window_callback);

	glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
	transformLoc = glGetUniformLocation(programID, "transform");
	colorLoc = glGetUniformLocation(programID, "color");

	// Orthographic projection in map squares: left=0, right=40, top=0, bottom=30
	projection = glm::ortho(0.0f, MAP_W, MAP_H, 0.0f, -1.0f, 1.0f);

	createQuad();
	createCircle();
	initText();          // bake the font into a texture
	initChallenges();
	restartGame();

	float lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float time = (float)glfwGetTime();
		float deltaTime = time - lastFrame;
		lastFrame = time;

		glfwPollEvents();
		updateGame(window, deltaTime, time);

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);

		drawMap(time);
		drawChallengeObjects(time);
		drawCharacters(time);
		drawRelationshipHUD();
		drawChallengeHUD();
		drawDialogBox();
		drawMessage();
		drawEndScreen();

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &quadVao);
	glDeleteVertexArrays(1, &circleVao);
	deleteText();
	glDeleteProgram(programID);
	glfwTerminate();
	return 0;
}
