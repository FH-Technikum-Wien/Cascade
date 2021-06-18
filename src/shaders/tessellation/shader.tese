#version 460 core
// equal_spacing: edge segments of equal length
// ccw: emit triangles in counter-clockwise order
layout(triangles, equal_spacing, ccw) in;

in CS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} cs_in[];

out ES_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} es_out;

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
    es_out.FragPos = interpolate3D(cs_in[0].FragPos, cs_in[1].FragPos, cs_in[2].FragPos);
    es_out.TexCoords = interpolate2D(cs_in[0].TexCoords, cs_in[1].TexCoords, cs_in[2].TexCoords);
    es_out.TangentNormal = normalize(interpolate3D(cs_in[0].TangentNormal, cs_in[1].TangentNormal, cs_in[2].TangentNormal));

    es_out.TangentLightPos = cs_in[0].TangentLightPos;
    es_out.TangentViewPos = cs_in[0].TangentViewPos;
    es_out.TangentFragPos = cs_in[0].TangentFragPos;
    es_out.FragPosLightSpace = cs_in[0].FragPosLightSpace;

	float displacement = texture(displacementMap, es_out.TexCoords.xy).x;
	es_out.FragPos += es_out.TangentNormal * displacement * displacementFactor;

	gl_Position = projectionMat * viewMat * vec4(es_out.FragPos, 1.0);
}
