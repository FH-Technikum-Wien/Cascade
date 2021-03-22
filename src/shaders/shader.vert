#version 430
layout(binding = 0) uniform sampler3D tex3D;

uniform int width;
uniform int height;
uniform int depth;
uniform float solidThreshold;

uniform float chunkHeight;

out vec3 pos;
out int mcIndex;


void main()
{
	// Retrieve position from ID
	int vertexId = gl_VertexID;
	int x = vertexId % width;
	int y = (vertexId / width) % height;
	int z = vertexId / (width * height);

	// Set vertex position (using chunk height)
	gl_Position = vec4(vec3(x, y + chunkHeight, z), 1.0);
	float xStep = 1.0 / (width - 1);
	float yStep = 1.0 / (height - 1);
	float zStep = 1.0 / (depth - 1);
	pos = vec3(x * xStep, y * yStep, z * zStep);

	// Calculate all marching cube indices with the corners
	int c1 = int(texture(tex3D, pos).r < solidThreshold);
    int c2 = int(texture(tex3D, pos + vec3(xStep, 0.0, 0.0)).r < solidThreshold);
    int c3 = int(texture(tex3D, pos + vec3(xStep, 0.0, zStep)).r < solidThreshold);
    int c4 = int(texture(tex3D, pos + vec3(0.0, 0.0, zStep)).r < solidThreshold);
    int c5 = int(texture(tex3D, pos + vec3(0.0, yStep, 0.0)).r < solidThreshold);
    int c6 = int(texture(tex3D, pos + vec3(xStep, yStep, 0.0)).r < solidThreshold);
    int c7 = int(texture(tex3D, pos + vec3(xStep, yStep, zStep)).r < solidThreshold);
    int c8 = int(texture(tex3D, pos + vec3(0.0, yStep, zStep)).r < solidThreshold);
    mcIndex = (c1 << 7) | (c2 << 6) | (c3 << 5) | (c4 << 4) | (c5 << 3) | (c6 << 2) | (c7 << 1) | c8;
}
