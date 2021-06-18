#version 460 core
// Number of Control Points in the output patch
layout (vertices = 3) out;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} vs_in[];

out CS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} cs_out[];

uniform vec3 cameraPos;

uniform float tesselationAmount;

void main()
{
    // Copy input CPs into output CPs
    cs_out[gl_InvocationID].FragPos = vs_in[gl_InvocationID].FragPos;
    cs_out[gl_InvocationID].TexCoords = vs_in[gl_InvocationID].TexCoords;
    cs_out[gl_InvocationID].TangentLightPos = vs_in[gl_InvocationID].TangentLightPos;
    cs_out[gl_InvocationID].TangentViewPos = vs_in[gl_InvocationID].TangentViewPos;
    cs_out[gl_InvocationID].TangentFragPos = vs_in[gl_InvocationID].TangentFragPos;
    cs_out[gl_InvocationID].TangentNormal = vs_in[gl_InvocationID].TangentNormal;
    cs_out[gl_InvocationID].FragPosLightSpace = vs_in[gl_InvocationID].FragPosLightSpace;


	// Calculate Tesselation Levels
    gl_TessLevelOuter[0] = tesselationAmount;
    gl_TessLevelOuter[1] = tesselationAmount;
    gl_TessLevelOuter[2] = tesselationAmount;
    gl_TessLevelInner[0] = tesselationAmount;
}