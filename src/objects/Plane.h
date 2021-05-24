#pragma once
#include "Object.h"

class Plane : public Object
{

public:
	Plane(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale);
};