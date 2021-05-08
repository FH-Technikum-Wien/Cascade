#pragma once

#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

enum ShaderType
{
	VERTEX_SHADER = GL_VERTEX_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
	GEOMETRY_SHADER = GL_GEOMETRY_SHADER
};

class Shader
{
public:
	// ID of the program
	unsigned int shaderProgramID = -1;

	Shader() = default;

	~Shader()
	{
		glDeleteShader(shaderProgramID);
	}

	void addShader(const char* shaderPath, ShaderType type, bool shouldLinkProgram = true)
	{
		std::string shaderString = readFile(shaderPath);

		const char* shaderCode = shaderString.c_str();

		int success;
		char infoLog[512];

		// Create shader Object
		unsigned int shader;
		shader = glCreateShader(type);
		if (shaderProgramID == -1)
			shaderProgramID = glCreateProgram();

		// Attach shader source to shader object
		glShaderSource(shader, 1, &shaderCode, nullptr);
		// Compile shader (at run-time)
		glCompileShader(shader);
		// Check compilation
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// Shader program
		glAttachShader(shaderProgramID, shader);

		if (shouldLinkProgram)
			linkProgram();

		// Delete Shaders after linking, not needed anymore
		glDeleteShader(shader);

	}

	void activate() const
	{
		glUseProgram(shaderProgramID);
	}

	void linkProgram() const
	{
		int success;
		char infoLog[512];

		glLinkProgram(shaderProgramID);

		// Check compilation
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_ERROR\n" << infoLog << std::endl;
		}
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
	}
	void setVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, glm::value_ptr(value));
	}
	void setVec4(const std::string& name, glm::vec4 value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, glm::value_ptr(value));
	}
	void setMat4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	static std::string readFile(const char* filePath)
	{
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open())
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
			return "";
		}

		std::stringstream fileStringStream;
		// Read file's buffer into stream
		fileStringStream << fileStream.rdbuf();
		// Close file
		fileStream.close();

		return fileStringStream.str();
	}
};
