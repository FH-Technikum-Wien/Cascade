#version 430
layout(location = 0) in vec2 aPos;

uniform float layer;
uniform float chunkHeight;

// 3D position used for generating noise in fragment shader
out vec3 pos;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	// Pass 3D position with chunkHeight and layer as z to fragment shader
	pos = vec3(aPos.x, aPos.y + chunkHeight, layer);
}