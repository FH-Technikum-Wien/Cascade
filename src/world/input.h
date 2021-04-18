#pragma once

#include <GLFW/glfw3.h>
#include "camera.h"

class Input
{
public:
    inline static float Bumpiness = 1.0f;

	static void ProcessContinuousInput(GLFWwindow* window, Camera* camera) 
	{
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::FORWARD);

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::BACKWARD);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::LEFT);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::RIGHT);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::UP);

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessInput(CameraMovement::DOWN);
	}

    static void ProcessScrollInput(GLFWwindow* window, double xoffset, double yoffset)
    {
        float stepsSize = Bumpiness <= 0.5f ? 0.05f : 0.1f;
        float newBumpiness = (yoffset > 0 ? stepsSize : -stepsSize) + Bumpiness;
        newBumpiness = std::min(2.0f, std::max(0.0f, newBumpiness));
        Bumpiness = newBumpiness;
    }

};
