#version 460 core
// equal_spacing: edge segments of equal length
// ccw: emit triangles in counter-clockwise order
layout(triangles, equal_spacing, ccw) in;

in vec3 FragPos_CS[];
in vec2 TexCoord_CS[];
in vec3 Normal_CS[];

out vec3 FragPos_ES;
out vec2 TexCoord_ES;
out vec3 Normal_ES;

// Local to World
uniform mat4 modelMat;
// World to View (Camera)
uniform mat4 viewMat;
// View to Clip (perspective)
uniform mat4 projectionMat;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;

uniform sampler2D displacementMap;
uniform float displacementFactor;


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}


void main()
{
	// Interpolate using barycentric coordinates
	FragPos_ES = interpolate3D(FragPos_CS[0], FragPos_CS[1], FragPos_CS[2]);
	TexCoord_ES = interpolate2D(TexCoord_CS[0], TexCoord_CS[1], TexCoord_CS[2]);
	Normal_ES = normalize(interpolate3D(Normal_CS[0], Normal_CS[1], Normal_CS[2]));

	float displacement = texture(displacementMap, TexCoord_ES.xy).x;
	FragPos_ES += Normal_ES * displacement * displacementFactor;

	gl_Position = projectionMat * viewMat * vec4(FragPos_ES, 1.0);
}
