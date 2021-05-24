#include "Cube.h"

Cube::Cube(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale) : Object(material, position, eulerAngles, scale)
{
	glm::vec3 texScale = glm::vec3(scale);

	vertices = new float[108]{
		// back face
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		// front face
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		// left face
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		// right face
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 // bottom face
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		// top face
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
	};

	normals = new float[108]{
		// back face
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		// front face
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		// left face
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		// right face
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		// bottom face
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		// top face
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
	};

	uvs = new float[72]{
		// back face
		0.0f, 0.0f,
		texScale.x, 0.0f,
		texScale.x, texScale.y,
		texScale.x, texScale.y,
		0.0f, texScale.y,
		0.0f, 0.0f,
		// front face
		0.0f, 0.0f,
		texScale.x, 0.0f,
		texScale.x, texScale.y,
		texScale.x, texScale.y,
		0.0f, texScale.y,
		0.0f, 0.0f,
		// left face
		0.0f, 0.0f,
		texScale.z, 0.0f,
		texScale.z, texScale.y,
		texScale.z, texScale.y,
		0.0f, texScale.y,
		0.0f, 0.0f,
		// right face
		0.0f, 0.0f,
		texScale.z, 0.0f,
		texScale.z, texScale.y,
		texScale.z, texScale.y,
		0.0f, texScale.y,
		0.0f, 0.0f,
		// bottom face
		0.0f, 0.0f,
		texScale.x, 0.0f,
		texScale.x, texScale.z,
		texScale.x, texScale.z,
		0.0f, texScale.z,
		0.0f, 0.0f,
		// top face
		0.0f, 0.0f,
		texScale.x, 0.0f,
		texScale.x, texScale.z,
		texScale.x, texScale.z,
		0.0f, texScale.z,
		0.0f, 0.0f,
	};

	indices = new unsigned int[36]{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	vertexCount = 36;
	indexCount = 36;

	initialize();
}
