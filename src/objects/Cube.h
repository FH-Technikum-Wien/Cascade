#pragma once
#include "Object.h"

class Cube : public Object
{
public:
	Cube(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale);
};

