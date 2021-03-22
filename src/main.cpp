#include <stdexcept>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders/shader.h"
#include "world/camera.h"
#include "world/input.h"
#include "world/chunk.h"
#include <vector>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define TEXTURE_DEPTH 64

ChunkDimensions chunkDimensions = { TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_DEPTH };

#define SOLID_THRESHOLD 0.51f

GLFWwindow* window = nullptr;
Shader shader = Shader();
Shader noiseShader = Shader();
Camera camera = Camera(glm::vec3(32.0f, 32.0f, 128.0f), 0.5f);

unsigned int VAO = 0;
unsigned int VBO = 0;

const char* NOISE_VERTEX_SHADER_PATH = "src/shaders/noiseShader.vert";
const char* NOISE_FRAGMENT_SHADER_PATH = "src/shaders/noiseShader.frag";

const char* VERTEX_SHADER_PATH = "src/shaders/shader.vert";
const char* FRAGMENT_SHADER_PATH = "src/shaders/shader.frag";
const char* GEOMETRY_SHADER_PATH = "src/shaders/shader.geom";

bool wireframeModeActive = false;

std::vector<Chunk> chunks = std::vector<Chunk>();

// Rendered on each layer
float rectangle[12] = 
{
	-1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f, -1.0f,

     1.0f,  1.0f,
	-1.0f,  1.0f,
	 1.0f, -1.0f,
};


void setupGLFW();
void setupData();
void printError();
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() 
{
	setupGLFW();
	setupData();

	glDisable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		printError();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// Sets one color for window (background).
		glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
		// Clear color buffer and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Handle iput
		Input::ProcessContinuousInput(window, &camera);

		if (camera.Position.y > chunks[2].ChunkHeight) 
		{
			// Switch positions, so the lowest chunk becomes the highest
			std::swap(chunks[0], chunks[1]);
			std::swap(chunks[1], chunks[2]);

			chunks[2].UpdateTexture3D(noiseShader, chunks[1].ChunkHeight + TEXTURE_HEIGHT);
		}
		else if(camera.Position.y < chunks[1].ChunkHeight)
		{
			// Switch positions, so the highest chunk becomes the lowest
			std::swap(chunks[2], chunks[1]);
			std::swap(chunks[1], chunks[0]);

			chunks[0].UpdateTexture3D(noiseShader, chunks[1].ChunkHeight - TEXTURE_HEIGHT);
		}

		shader.activate();
		shader.setMat4("viewMat", camera.GetViewMat());
		for (const Chunk& chunk : chunks)
		{
			chunk.RenderPoints(shader, wireframeModeActive);
		}

		// Swaps the drawn buffer with the buffer that got written to.
		glfwSwapBuffers(window);
		// Checks if any events are triggered and executes callbacks.
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void setupGLFW()
{
	glfwInit();
	// Tell GLFW that we're using OpenGL version 3.3 .
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW to use Core_Profile -> Smaller subset without backwards-compatibility (not needed).
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

	// Set OpenGL viewport.
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	// Events
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, keyPressedCallback);
	// Lock cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void setupData() 
{
	// Projection Matrix for adding perspective.
	glm::mat4 projectionMat;
	projectionMat = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

	shader.addShader(VERTEX_SHADER_PATH, VERTEX_SHADER);
	shader.addShader(FRAGMENT_SHADER_PATH, FRAGMENT_SHADER);
	shader.addShader(GEOMETRY_SHADER_PATH, GEOMETRY_SHADER);

	shader.activate();
	shader.setMat4("projectionMat", projectionMat);
	shader.setInt("width", TEXTURE_WIDTH);
	shader.setInt("height", TEXTURE_HEIGHT);
	shader.setInt("depth", TEXTURE_DEPTH);
	shader.setFloat("solidThreshold", SOLID_THRESHOLD);


	//----------------
	// Vertex data
	//----------------
	noiseShader.addShader(NOISE_VERTEX_SHADER_PATH, VERTEX_SHADER);
	noiseShader.addShader(NOISE_FRAGMENT_SHADER_PATH, FRAGMENT_SHADER);

	glGenVertexArrays(1, &VAO);
	// Bind Vertex-Array-Cube to configure VBO(s)
	glBindVertexArray(VAO);
	// Generate Vertex-Buffer-Cube to manage memory on GPU and store vertices
	glGenBuffers(1, &VBO);
	// Bind Vertex-Buffer-Cube to configure it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy vertex data into buffer's memory (into VBO which is bound to GL_ARRAY_BUFFER)
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), rectangle, GL_STATIC_DRAW);

	// Tell OpenGL how to interpret/read the vertex data (per vertex attribute, e.g. one vertex point)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	chunks.push_back(Chunk(chunkDimensions, -TEXTURE_HEIGHT, noiseShader, VAO, SCREEN_WIDTH, SCREEN_HEIGHT));
	chunks.push_back(Chunk(chunkDimensions,				  0, noiseShader, VAO, SCREEN_WIDTH, SCREEN_HEIGHT));
	chunks.push_back(Chunk(chunkDimensions,  TEXTURE_HEIGHT, noiseShader, VAO, SCREEN_WIDTH, SCREEN_HEIGHT));
}

void printError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) 
	{
		std::cout << "Error: " << error << std::endl;
	}
}

void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ProcessMouse(xPos, yPos);
}

void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		wireframeModeActive = !wireframeModeActive;
	}
}
