#pragma once

// Keyboard input

// Include GLEW and GLFW
#include "glew/glew.h"   
#include "glfw/glfw3.h"


const int TRACKED_KEYS[] = { GLFW_KEY_E, GLFW_KEY_Y, GLFW_KEY_N, GLFW_KEY_R, GLFW_KEY_1, GLFW_KEY_2 }; // Keys used for game interactions
bool keyNow[512] = { false };		// Store the current key states
bool keyBefore[512] = { false };	// Store the previous key states

// Update the keyboard states
void updateKeys(GLFWwindow* window)
{
	for (int k : TRACKED_KEYS) {
		keyBefore[k] = keyNow[k];							// Save the previous state
		keyNow[k] = glfwGetKey(window, k) == GLFW_PRESS;	// Get the current state
	}
}

bool keyPressed(int key) { return keyNow[key] && !keyBefore[key]; }	// Check if a key was just pressed
