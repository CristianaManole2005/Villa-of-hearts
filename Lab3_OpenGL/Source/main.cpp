#include <stdio.h>
#include <math.h>
#include <vector>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shaders/Utils/shaderLoader.hpp"
#include "Globals/globals.hpp"

// load images for textures
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// store the position, size and color of each body part
struct BodyPart {
	glm::vec2 pos;
	glm::vec2 scale;
	glm::vec4 color;
};

glm::vec2 characterOffset(0.0f, 0.0f);	// player position
glm::vec2 companionPos(-0.35f, -0.20f);	// companion position
glm::vec2 heartPos(0.45f, 0.35f);		// heart position
bool heartVisible = true;				// if the heart is still available

// variables for time and jumping
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float jumpTimer = 0.0f;

// keyboard input
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float speed = 1.0f * deltaTime;						// player movement with WASD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		characterOffset.y += speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		characterOffset.y -= speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		characterOffset.x -= speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		characterOffset.x += speed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && jumpTimer <= 0.0f)
		jumpTimer = 1.0f;
}

// draw the character using 5 body parts
void drawCharacter(glm::mat4 rootTransform, BodyPart parts[], glm::vec4 shirtColor, unsigned int transformLoc, unsigned int colorLoc)
{
	for (int i = 0; i < 5; i++)
	{
		// draw the black border
		glm::mat4 border = glm::translate(rootTransform, glm::vec3(parts[i].pos, 0.0f));
		border = glm::scale(border, glm::vec3(parts[i].scale + glm::vec2(0.015f, 0.015f), 1.0f));

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(border));
		glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::vec4 partColor = (i >= 1 && i <= 3) ? shirtColor : parts[i].color;			  // set the color for the body parts

		// draw the colored part inside the border
		glm::mat4 fill = glm::translate(rootTransform, glm::vec3(parts[i].pos, 0.0f));	  
		fill = glm::scale(fill, glm::vec3(parts[i].scale, 1.0f));

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(fill));
		glUniform4fv(colorLoc, 1, glm::value_ptr(partColor));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

int main()
{
	// Initialize GLFW
	if (!glfwInit()) return -1;

	// Create the game window
	window = glfwCreateWindow(windowWidth, windowHeight, "partOne project", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}

	// set OpenGL settings
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	// background color white

	// Load the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

	// vertices for a square
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// create VAO, VBO and IBO
	GLuint vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);	// store the square data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// vertices for the heart
	float heartVertices[] = {
		 0.00f, -0.06f, 0.0f, // bottom tip
		 0.05f, -0.01f, 0.0f, // lower right slope
		 0.06f,  0.03f, 0.0f, // right edge
		 0.03f,  0.06f, 0.0f, // top right arch
		 0.00f,  0.03f, 0.0f, // top center dip
		-0.03f,  0.06f, 0.0f, // top left arch
		-0.06f,  0.03f, 0.0f, // left edge
		-0.05f, -0.01f, 0.0f, // lower left slope
		 0.00f, -0.06f, 0.0f  // bottom tip 
	};

	// create VAO and VBO for the heart
	GLuint heartVao, heartVbo;
	glGenVertexArrays(1, &heartVao);
	glGenBuffers(1, &heartVbo);

	glBindVertexArray(heartVao);
	glBindBuffer(GL_ARRAY_BUFFER, heartVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(heartVertices), heartVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// create the character from rectangles
	BodyPart character[] = {
		{ glm::vec2(0.00f,  0.14f), glm::vec2(0.11f, 0.10f), glm::vec4(1.00f, 0.95f, 0.00f, 1.0f) }, // Head
		{ glm::vec2(0.00f, -0.01f), glm::vec2(0.11f, 0.17f), glm::vec4(0.00f, 0.65f, 0.90f, 1.0f) }, // Torso
		{ glm::vec2(-0.08f, 0.01f), glm::vec2(0.03f, 0.13f), glm::vec4(0.00f, 0.65f, 0.90f, 1.0f) }, // Left arm
		{ glm::vec2(0.08f,  0.01f), glm::vec2(0.03f, 0.13f), glm::vec4(0.00f, 0.65f, 0.90f, 1.0f) }, // Right arm
		{ glm::vec2(0.00f, -0.14f), glm::vec2(0.11f, 0.08f), glm::vec4(0.65f, 0.42f, 0.28f, 1.0f) }  // Legs
	};

	// get the variables from the shaders
	unsigned int transformLoc = glGetUniformLocation(programID, "transform");
	unsigned int colorLoc = glGetUniformLocation(programID, "objectColor");
	unsigned int bgTexLoc = glGetUniformLocation(programID, "bgTexture");
	unsigned int useTexLoc = glGetUniformLocation(programID, "useTexture");

	// create and configure background texture object
	unsigned int bgTexture;
	glGenTextures(1, &bgTexture);
	glBindTexture(GL_TEXTURE_2D, bgTexture);

	// set how the texture is displayed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load the villa map image
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("Resources/Images/map.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;	// check the image format
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);	// send the image to OpenGL
		stbi_image_free(data);	// free the image data
	}
	else
	{
		printf("ERROR: %s\n", stbi_failure_reason());
	}

	while (!glfwWindowShouldClose(window))
	{
		// update the viewport
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);

		// keep the objects in the correct proportions
		float aspect = (fbWidth > 0) ? (float)fbHeight / (float)fbWidth : 1.0f;
		glm::mat4 aspectFix = glm::scale(glm::mat4(1.0f), glm::vec3(aspect, 1.0f, 1.0f));

		// calculate the time between frames
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// process keyboard controls
		processInput(window);

		// compute parabolic jump height and 360-degree flip angle when SPACE is pressed
		float jumpY = 0.0f;
		float flipAngle = 0.0f;
		if (jumpTimer > 0.0f)
		{
			// update the jump timer
			jumpTimer = glm::max(0.0f, jumpTimer - deltaTime * 1.5f);
			float progress = 1.0f - jumpTimer;

			jumpY = 4.0f * progress * (1.0f - progress) * 0.35f;	// make the player jump using a parabola
			flipAngle = -progress * 360.0f;							// rotate the player during the jump
		}

		// companion follows the player using linear interpolation when outside arrival radius
		if (glm::distance(companionPos, characterOffset) > 0.28f)
		{
			companionPos = glm::mix(companionPos, characterOffset, 3.0f * deltaTime);
		}

		// clear color buffer and activate shader program
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);

		// draw the background map
		glUniform1i(useTexLoc, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bgTexture);
		glUniform1i(bgTexLoc, 0);

		// make the map cover the screen
		glm::mat4 bgTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(bgTransform));

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// use colors instead of textures
		glUniform1i(useTexLoc, 0);

		// draw the collectible heart
		if (heartVisible)
		{
			glm::vec2 currentHeartPos(heartPos.x, heartPos.y + sin(currentFrame * 3.0f) * 0.08f);	// the heart moves up and down
			glm::vec2 playerPos(characterOffset.x, characterOffset.y + jumpY);						// get the player position with the jump

			// check if the player is close enough to collect the heart
			if (glm::distance(playerPos, currentHeartPos) < 0.22f)
			{
				heartVisible = false;
			}
			else
			{
				glBindVertexArray(heartVao);

				// move and rotate the heart
				glm::mat4 heartTransform = glm::rotate(
					glm::translate(aspectFix, glm::vec3(currentHeartPos, 0.0f)),
					currentFrame * 120.0f,
					glm::vec3(0.0f, 1.0f, 0.0f)
				);

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(heartTransform));	// send the heart position to the shader
				glUniform4f(colorLoc, 0.90f, 0.10f, 0.20f, 1.0f);								// heart color red

				// draw outline
				glLineWidth(3.0f);
				glDrawArrays(GL_LINE_STRIP, 0, 9);
			}
		}

		glBindVertexArray(vao);	// use the square VAO for the characters

		// root transformation matrix for smaller follower companion
		glm::mat4 companionRoot = glm::translate(aspectFix, glm::vec3(companionPos, 0.0f));						// draw the companion
		companionRoot = glm::scale(companionRoot, glm::vec3(0.75f, 0.75f, 1.0f));								// make the companion smaller
		drawCharacter(companionRoot, character, glm::vec4(0.85f, 0.20f, 0.35f, 1.0f), transformLoc, colorLoc);	// draw the companion with a pink shirt

		// root transformation matrix for main player character
		glm::mat4 charRoot = glm::translate(aspectFix, glm::vec3(characterOffset.x, characterOffset.y + jumpY, 0.0f));
		charRoot = glm::rotate(charRoot, flipAngle, glm::vec3(0.0f, 0.0f, 1.0f));										// apply the rotation when jumping
		drawCharacter(charRoot, character, glm::vec4(0.00f, 0.65f, 0.90f, 1.0f), transformLoc, colorLoc);

		glfwSwapBuffers(window);
		glfwPollEvents();			
	}

	// delete textures and buffers
	glDeleteTextures(1, &bgTexture);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &heartVbo);
	glDeleteVertexArrays(1, &heartVao);
	glDeleteProgram(programID);
	glfwTerminate();

	return 0;
}