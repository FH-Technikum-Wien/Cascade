#include "Terrain.h"
#include <glm/gtx/transform.hpp>


Terrain::Terrain(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scaleFactor)
{
	translate(position);
	rotate(eulerAngles);
	scale(scaleFactor);

	this->material = material;
	glm::vec3 texScale = glm::vec3(scaleFactor);
	//scale = scale / glm::vec3(2.0f);

	vertices = new float[18]{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
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

	vertexCount = 6;
	indexCount = 6;

	initialize();
}

void Terrain::Render(const Shader& shader, bool wireframeMode)
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

	// Set render mode to wireframe
	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(VAO);
	shader.setMat4("modelMat", transform);
	//shader.setFloat("ambientStrength", material.ambientStrength);
	//shader.setFloat("diffuseStrength", material.diffuseStrength);
	//shader.setFloat("specularStrength", material.specularStrength);
	//shader.setFloat("focus", material.focus);
	//shader.setVec3("textureColor", material.color);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTICES);
	glDrawElements(GL_PATCHES, indexCount, GL_UNSIGNED_INT, 0);

	// Reset render mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void Terrain::initialize()
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

	glBindVertexArray(0);
}

void Terrain::translate(glm::vec3 translation)
{
	transform = glm::translate(transform, translation);
}

void Terrain::scale(glm::vec3 scaleFactor)
{
	transform = glm::scale(transform, scaleFactor);
}

void Terrain::rotate(glm::vec3 eulerAngles)
{
	transform = glm::rotate(transform, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
}
