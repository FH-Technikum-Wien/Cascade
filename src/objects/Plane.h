#pragma once

#include <glad\glad.h>
#include <glm/matrix.hpp>

#include "Material.h"

class Plane
{
public:
	glm::mat4 transform = glm::mat4(1.0f);

	Material material;
	float* vertices;
	float* normals;
	float* uvs;
	unsigned int* indices;
	float* tangents;

public:
	Plane() = default;

	Plane(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale)
	{
		translate(position);
		rotate(eulerAngles);

		this->material = material;
		glm::vec3 texScale = glm::vec3(scale);
		//scale = scale / glm::vec3(2.0f);

		vertices = new float[18]{
			-scale.x,  scale.y, 0.0f,
			-scale.x, -scale.y, 0.0f,
			 scale.x, -scale.y, 0.0f,
			-scale.x,  scale.y, 0.0f,
			 scale.x, -scale.y, 0.0f,
			 scale.x,  scale.y, 0.0f,
		};

		normals = new float[18]{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};

		uvs = new float[12]{
				  0.0f, texScale.y,
				  0.0f,       0.0f,
			texScale.x,       0.0f,
				  0.0f, texScale.y,
			texScale.x,       0.0f,
			texScale.x, texScale.y,
		};

		indices = new unsigned int[6]{
			0,1,2,
			3,4,5
		};

		initialize();
	}

	void render(const Shader& shader)
	{
		// Add texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.texture);

		// Add normalMap
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.normalMap);

		// Add displacement
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.displacementMap);

		glBindVertexArray(VAO);
		shader.setMat4("modelMat", transform);
		shader.setFloat("ambientStrength", material.ambientStrength);
		shader.setFloat("diffuseStrength", material.diffuseStrength);
		shader.setFloat("specularStrength", material.specularStrength);
		shader.setFloat("focus", material.focus);
		shader.setVec3("textureColor", material.color);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTICES);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}

private:
	unsigned int VAO = 0;
	unsigned int EBO = 0;
	unsigned int VBO_VERTICES = 0;
	unsigned int VBO_NORMALS = 0;
	unsigned int VBO_UVS = 0;
	unsigned int VBO_TANGENTS = 0;

	unsigned int vertexCount = 6;
	unsigned int indexCount = 6;

private:
	void initialize()
	{
		// Generate Vertex-Array-Cube to store vertex attribute configuration and which VBO(s) to use
		glGenVertexArrays(1, &VAO);
		// Bind Vertex-Array-Cube to configure VBO(s)
		glBindVertexArray(VAO);

		// Generate Vertex-Buffer-Cube to manage memory on GPU and store vertices
		glGenBuffers(1, &VBO_VERTICES);
		// Bind Vertex-Buffer-Cube to configure it
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTICES);
		// Copy vertex data into buffer's memory (into VBO which is bound to GL_ARRAY_BUFFER)
		glBufferData(GL_ARRAY_BUFFER, (vertexCount * 3) * sizeof(float), vertices, GL_STATIC_DRAW);
		// POSITION
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);


		
		// NORMALS
		glGenBuffers(1, &VBO_NORMALS);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMALS);
		glBufferData(GL_ARRAY_BUFFER, (vertexCount * 3) * sizeof(float), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		// UVs
		glGenBuffers(1, &VBO_UVS);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_UVS);
		glBufferData(GL_ARRAY_BUFFER, (vertexCount * 2) * sizeof(float), uvs, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		// TANGENTS
		calculateTangents();
		glGenBuffers(1, &VBO_TANGENTS);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_TANGENTS);
		glBufferData(GL_ARRAY_BUFFER, (vertexCount * 3) * sizeof(float), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);
	}

	void calculateTangents()
	{
		tangents = new float[vertexCount * 3];
		unsigned int tangentIndex = 0;
		for (unsigned int i = 0; i < vertexCount / 3; i++)
		{
			// Get vertices
			unsigned int vertex1 = i * 9;
			unsigned int vertex2 = i * 9 + 3;
			unsigned int vertex3 = i * 9 + 6;
			glm::vec3 pos1 = glm::vec3(vertices[vertex1], vertices[vertex1 + 1], vertices[vertex1 + 2]);
			glm::vec3 pos2 = glm::vec3(vertices[vertex2], vertices[vertex2 + 1], vertices[vertex2 + 2]);
			glm::vec3 pos3 = glm::vec3(vertices[vertex3], vertices[vertex3 + 1], vertices[vertex3 + 2]);

			// Calculate edges
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;

			// Get UV's
			unsigned int uvIndex1 = i * 6;
			unsigned int uvIndex2 = i * 6 + 2;
			unsigned int uvIndex3 = i * 6 + 4;
			glm::vec2 uv1 = glm::vec2(uvs[uvIndex1], uvs[uvIndex1 + 1]);
			glm::vec2 uv2 = glm::vec2(uvs[uvIndex2], uvs[uvIndex2 + 1]);
			glm::vec2 uv3 = glm::vec2(uvs[uvIndex3], uvs[uvIndex3 + 1]);

			// Calculate deltaUV's
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			// Calculate x,y,z for the tangents
			glm::vec3 tangent = glm::vec3();
			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);

			// All tangents same for triangle.
			for (unsigned int j = 0; j < 3; j++)
			{
				tangents[tangentIndex] = tangent.x;
				tangents[tangentIndex + 1] = tangent.y;
				tangents[tangentIndex + 2] = tangent.z;
				tangentIndex += 3;
			}
		}
	}

	void translate(glm::vec3 translation)
	{
		transform = glm::translate(transform, translation);
	}

	void rotate(glm::vec3 eulerAngles)
	{
		transform = glm::rotate(transform, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}
};