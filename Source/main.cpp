#pragma once

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

// Game files
#include "Game/Config.hpp"       // map size, player settings
#include "Game/Colors.hpp"       // all colors
#include "Game/PixelArt.hpp"     // heart and character bitmaps
#include "Game/Renderer.hpp"     // OpenGL setup, drawRect, drawCircle, drawHeart... (Functions for drawing shapes)
#include "Game/TextRenderer.hpp" // text with the Press Start 2P font (stb_truetype) (Display text on the screen)
#include "Game/World.hpp"        // map objects and collision checks
#include "Game/Challenges.hpp"   // the 6 challenges and their data
#include "Game/Npcs.hpp"         // NPCs(Gabriela & Bianca) and relationships
#include "Game/GameState.hpp"    // everything that changes while playing
#include "Game/Input.hpp"        // keyboard input
#include "Game/GameLogic.hpp"    // collisions, dialogue, challenge rules, movement
#include "Game/GameDraw.hpp"     // drawing the map, challenges, characters, UI

int main(void)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

	//for macOS
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the game window
	window = glfwCreateWindow(windowWidth, windowHeight, "Villa of hearts: Make Them Yours", NULL, NULL);

	// Check if the window was created
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// Set the OpenGL context
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// Set the viewport size
	int fbw, fbh;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	setViewport(fbw, fbh);
	glfwSetFramebufferSizeCallback(window, window_callback); // Update the viewport when resizing the window

	glClearColor(0.05f, 0.05f, 0.08f, 1.0f);  // Set the background color

	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag"); // Load the shaders
	transformLoc = glGetUniformLocation(programID, "transform");
	colorLoc = glGetUniformLocation(programID, "color");

	// Orthographic projection in map squares: left=0, right=40, top=0, bottom=30
	projection = glm::ortho(0.0f, MAP_W, MAP_H, 0.0f, -1.0f, 1.0f);

	// Create the shapes used in the game
	createQuad();
	createCircle();

	// Initialize the text and challenges
	initText();
	initChallenges();
	restartGame(); // Set the game to its starting state

	float lastFrame = (float)glfwGetTime(); // Save the time of the previous frame

	while (!glfwWindowShouldClose(window))
	{
		// Calculate the time between frames
		float time = (float)glfwGetTime();
		float deltaTime = time - lastFrame;
		lastFrame = time;

		glfwPollEvents(); // Check keyboard and window events
		updateGame(window, deltaTime, time); // Update player movement and game events

		glClear(GL_COLOR_BUFFER_BIT); // Clear the screen before drawing
		glUseProgram(programID);

		// Draw the map and its objects
		drawMap(time);
		drawChallengeObjects(time);
		drawCharacters(time);  // Draw the player and NPCs
		drawRelationshipHUD(); // Draw the relationship scores
		drawChallengeHUD();    // Draw the current challenge
		// Draw dialogue and messages
		drawDialogBox();
		drawMessage();
		drawEndScreen();       // Draw the ending screen when the game finishes

		glfwSwapBuffers(window); // Display the new frame
	}

	glDeleteVertexArrays(1, &quadVao);
	glDeleteVertexArrays(1, &circleVao);
	deleteText();               // Delete the text resources
	glDeleteProgram(programID);
	glfwTerminate();
	return 0;
}
