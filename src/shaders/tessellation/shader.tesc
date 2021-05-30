#version 460 core

// Number of Control Points in the output patch
layout (vertices = 3) out;

uniform vec3 cameraPos;

in vec3 FragPos_VS[];
in vec2 TexCoord_VS[];
in vec3 Normal_VS[];

out vec3 FragPos_CS[];
out vec2 TexCoord_CS[];
out vec3 Normal_CS[];


float GetTessLevel(float distance0, float distance1)
{
	float averageDistance = (distance0 + distance1) / 2.0;

	if(averageDistance <= 2.0)
    {
		return 10.0;
    }
	else if(averageDistance <= 5.0)
    {
		return 7.0;
    }
    else
    {
		return 3.0;
    }
}

void main()
{
	// Copy input CPs into output CPs
    FragPos_CS[gl_InvocationID] = FragPos_VS[gl_InvocationID];
    TexCoord_CS[gl_InvocationID] = TexCoord_VS[gl_InvocationID];
    Normal_CS[gl_InvocationID] = Normal_VS[gl_InvocationID];

	// Distance from Camera to the three control points
	float cameraToVertexDistance0 = distance(cameraPos, FragPos_CS[0]);
	float cameraToVertexDistance1 = distance(cameraPos, FragPos_CS[1]);
	float cameraToVertexDistance2 = distance(cameraPos, FragPos_CS[2]);

	// Calculate Tesselation Levels
	// TessLevelOuter ~ number of segments on each edge
	gl_TessLevelOuter[0] = GetTessLevel(cameraToVertexDistance1, cameraToVertexDistance2);
	gl_TessLevelOuter[1] = GetTessLevel(cameraToVertexDistance2, cameraToVertexDistance0);
	gl_TessLevelOuter[2] = GetTessLevel(cameraToVertexDistance0, cameraToVertexDistance1);
	// TessLevelInner ~ number of rings the triangles contain
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}