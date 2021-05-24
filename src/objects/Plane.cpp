#include "Plane.h"
#include <glm/gtx/transform.hpp>


Plane::Plane(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale) : Object(material, position, eulerAngles, scale)
{
	this->material = material;
	glm::vec3 texScale = glm::vec3(scale);
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
