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

float GetTessLevel(float distance0, float distance1)
{
	float averageDistance = (distance0 + distance1) / 2.0;

	if(averageDistance <= 3.0)
    {
		return tesselationAmount * 10;
    }
	else if(averageDistance <= 6.0)
    {
		return tesselationAmount * 5;
    }
    else if(averageDistance <= 10.0)
    {
		return tesselationAmount * 2.5;
    }
    else
    {
        return tesselationAmount;
    }
}

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

	

	// Distance from Camera to the three control points
	float cameraToVertexDistance0 = distance(cameraPos, cs_out[0].FragPos);
	float cameraToVertexDistance1 = distance(cameraPos, cs_out[1].FragPos);
	float cameraToVertexDistance2 = distance(cameraPos, cs_out[2].FragPos);

	// Calculate Tesselation Levels
	// TessLevelOuter ~ number of segments on each edge
	gl_TessLevelOuter[0] = GetTessLevel(cameraToVertexDistance1, cameraToVertexDistance2);
	gl_TessLevelOuter[1] = GetTessLevel(cameraToVertexDistance2, cameraToVertexDistance0);
	gl_TessLevelOuter[2] = GetTessLevel(cameraToVertexDistance0, cameraToVertexDistance1);
	// TessLevelInner ~ number of rings the triangles contain
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}