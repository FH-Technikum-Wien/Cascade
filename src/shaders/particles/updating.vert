#version 460

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 pVelocity;
layout (location = 2) in vec3 pColor;
layout (location = 3) in float pLifetime;
layout (location = 4) in float pSize;
layout (location = 5) in float pType;

// Simply pass data to geometry shader
out vec3 pPositionPass;
out vec3 pVelocityPass;
out vec3 pColorPass;
out float pLifetimePass;
out float pSizePass;
out float pTypePass;

void main() 
{
	pPositionPass = pPosition;
	pVelocityPass = pVelocity;
	pColorPass = pColor;
	pLifetimePass = pLifetime;
	pSizePass = pSize;
	pTypePass = pType;
}