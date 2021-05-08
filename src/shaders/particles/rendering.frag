#version 460

layout (binding = 0) uniform sampler2D diffuseTexture;

smooth in vec2 TexCoord;
flat in vec4 Color;

out vec4 FragColor;

void main() 
{
	//FragColor = vec4(texture2D(diffuseTexture, TexCoord).xyz, 1.0) * Color;
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}