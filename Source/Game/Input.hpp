#pragma once
// Input.hpp
// Keyboard helper

#include "glew/glew.h"   // GLEW must come before GLFW
#include "glfw/glfw3.h"

// ------------------------------------------------------------
// Keyboard: "pressed" is true only on the frame the key goes down
// ------------------------------------------------------------
const int TRACKED_KEYS[] = { GLFW_KEY_E, GLFW_KEY_Y, GLFW_KEY_N, GLFW_KEY_R, GLFW_KEY_1, GLFW_KEY_2 };
bool keyNow[512] = { false };
bool keyBefore[512] = { false };

void updateKeys(GLFWwindow* window)
{
	for (int k : TRACKED_KEYS) {
		keyBefore[k] = keyNow[k];
		keyNow[k] = glfwGetKey(window, k) == GLFW_PRESS;
	}
}

bool keyPressed(int key) { return keyNow[key] && !keyBefore[key]; }
