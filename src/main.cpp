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
#include "intersection/KdTree.h"

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

Plane ground;
KdTree* kdTree;

void setupGLFW();
void setupKdTree();

void HandleMouseClick();

void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void printError();





int main()
{
	setupGLFW();

	proceduralSystem = new ProceduralSystem(SCREEN_WIDTH, SCREEN_HEIGHT, camera);
	displacementSystem = new DisplacementSystem(camera, light);
	particleSystem = new ParticleSystem(camera);

	Shader groundShader = Shader();
	groundShader.addShader("src/shaders/sampleShader.vert", ShaderType::VERTEX_SHADER);
	groundShader.addShader("src/shaders/sampleShader.frag", ShaderType::FRAGMENT_SHADER);

	Material groundMat = Material("art/brickWall.jpg", "art/brickWall_normal.jpg", GL_RGB);
	ground = Plane(groundMat, glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(-90.0f,0.0f,0.0f), glm::vec3(10));

	setupKdTree();

	std::chrono::high_resolution_clock clock;
	auto lastFrameTime = clock.now();

	float timePressed = 0;
	float delay = 0.5f;

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

		groundShader.activate();
		groundShader.setInt("diffuseTexture", 0);
		groundShader.setInt("normalMap", 1);
		groundShader.setMat4("projectionMat", camera.ProjectionMat);
		groundShader.setFloat("ambientLightAmount", 1.0f);
		groundShader.setMat4("viewMat", camera.GetViewMat());
		groundShader.setVec3("cameraPos", camera.Position);
		groundShader.setFloat("bumpiness", Input::Bumpiness);
		ground.Render(groundShader, false);

		proceduralSystem->Update(camera);
		displacementSystem->Update(camera, false);

		particleSystem->Update(camera, deltaTime);
		particleSystem->Render(camera);

		// Handle iput
		Input::ProcessContinuousInput(window, &camera);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && timePressed + delay < glfwGetTime())
		{
			timePressed = glfwGetTime();
			HandleMouseClick();
		}


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

void setupKdTree()
{
	// Kd-Tree
	std::cout << "\n[*] Building kd-tree (slow)" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	kdTree = new KdTree(ground.GetVerticesInWorldSpace(), 6);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "[->] Done!" << std::endl;
	std::cout << "Building time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	kdTree->printStatistics();
}

void HandleMouseClick()
{
	KdStructs::Vector position = KdStructs::Vector(camera.Position.x, camera.Position.y, camera.Position.z);
	glm::vec3 directionVector = camera.Orientation * glm::vec3(0, 0, -1);
	// Normalize
	directionVector = glm::normalize(directionVector);
	KdStructs::Vector direction = KdStructs::Vector(directionVector.x, directionVector.y, directionVector.z);

	// Cast ray into scene
	KdStructs::RayHit* hit = nullptr;

	std::cout << "\n[*] Casting Ray." << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	kdTree->raycast(KdStructs::Ray(position, direction, 1000), hit);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Raycast time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	std::cout << std::endl;

	if (hit != nullptr)
		particleSystem->SpawnPosition = glm::vec3(hit->position[0], hit->position[1], hit->position[2]);
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