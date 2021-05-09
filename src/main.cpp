#include <stdexcept>
#include <vector>
#include <chrono>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "objects/Material.h"
#include "objects/Plane.h"
#include "world/Camera.h"
#include "world/Input.h"
#include "world/Chunk.h"
#include "world/Light.h"

#include "world/ProceduralSystem.h"
#include "world/DisplacementSystem.h"
#include "world/ParticleSystem.h"

// Defines the glfw window size
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f

GLFWwindow* window = nullptr;
Camera camera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f), 0.1f);
Light light = Light(glm::vec3(0.2f, 0.5f, 0.2f), 1.0f);

bool wireframeModeActive = false;

ProceduralSystem* proceduralSystem;
DisplacementSystem* displacementSystem;
ParticleSystem* particleSystem;

void setupGLFW();

void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void printError();





int main()
{
	setupGLFW();

	proceduralSystem = new ProceduralSystem(SCREEN_WIDTH, SCREEN_HEIGHT, camera);
	displacementSystem = new DisplacementSystem(camera, light);
	particleSystem = new ParticleSystem(camera);

	std::chrono::high_resolution_clock clock;
	auto lastFrameTime = clock.now();

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime in seconds
		auto currentFrameTime = clock.now();
		double deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentFrameTime - lastFrameTime).count() / 1e9;
		lastFrameTime = currentFrameTime;

		printError();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// Sets one color for window (background)
		glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
		// Clear color buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		proceduralSystem->Update(camera);
		displacementSystem->Update(camera, false);

		particleSystem->Update(camera, deltaTime);
		particleSystem->Render(camera);

		// Handle iput
		Input::ProcessContinuousInput(window, &camera);


		std::string lastInput =
			"Bumpiness: " + std::to_string(Input::Bumpiness) +
			" | HeightScale: " + std::to_string(displacementSystem->HeightScale) +
			" | Steps: " + std::to_string(displacementSystem->Steps) +
			" | Refinement Steps: " + std::to_string(displacementSystem->RefinementSteps);
		glfwSetWindowTitle(window, lastInput.c_str());

		// Swaps the drawn buffer with the buffer that got written to
		glfwSwapBuffers(window);
		// Checks if any events are triggered and executes callbacks
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void setupGLFW()
{
	glfwInit();
	// Tell GLFW that we're using OpenGL version 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// Tell GLFW to use Core_Profile -> Smaller subset without backwards-compatibility (not needed)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW-Window");
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// Set OpenGL viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	// Events
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, keyPressedCallback);
	glfwSetScrollCallback(window, Input::ProcessScrollInput);
	// Lock cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << glGetString(GL_VERSION) << std::endl;
}

#pragma region Input
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ProcessMouse(xPos, yPos);
}

void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		wireframeModeActive = !wireframeModeActive;

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		displacementSystem->HeightScale -= displacementSystem->HeightScaleSteps;

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		displacementSystem->HeightScale += displacementSystem->HeightScaleSteps;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		++displacementSystem->Steps;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		--displacementSystem->Steps;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		++displacementSystem->RefinementSteps;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		--displacementSystem->RefinementSteps;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		camera.MovementSpeed = 0.01f;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		camera.MovementSpeed = 0.1f;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		camera.MovementSpeed = 1.0f;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		camera.MovementSpeed = 10.0f;
}
#pragma endregion


void printError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "Error: " << error << std::endl;
	}
}