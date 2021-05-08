#version 460

layout (location = 0) in vec3 pPosition;
// No need for velocity here
layout (location = 2) in vec3 pColor;
layout (location = 3) in float pLifetime;
layout (location = 4) in float pSize;
layout (location = 5) in float pType;

// Simply pass data to geometry shader
out vec3 pColorPass;
out float pLifetimePass;
out float pSizePass;
out float pTypePass;

void main() 
{
	gl_Position = vec4(pPosition, 1.0);
	pColorPass = pColor;
	pLifetimePass = pLifetime;
	pSizePass = pSize;
	pTypePass = pType;
}