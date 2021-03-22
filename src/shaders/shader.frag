#version 430
layout(binding = 0) uniform sampler3D tex3D;

in vec3 mcPos;

out vec4 FragColor;

void main()
{
	// Use position to determine color from noise texture
	float c = texture(tex3D, mcPos).r;
	FragColor = vec4(c,c,c,1.0);
}