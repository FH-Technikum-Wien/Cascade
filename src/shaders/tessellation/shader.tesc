#version 460 core
// Number of Control Points in the output patch
layout (vertices = 1) out;

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

struct OUTPUT_PATCH{
    vec3 FragPos_030;
    vec3 FragPos_021;
    vec3 FragPos_012;
    vec3 FragPos_003;
    vec3 FragPos_102;
    vec3 FragPos_201;
    vec3 FragPos_300;
    vec3 FragPos_210;
    vec3 FragPos_120;
    vec3 FragPos_111;
    vec3 Normal[3];
    vec2 TexCoords[3];
};

// Attributes of the output patch
out patch OUTPUT_PATCH output_patch;

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


vec3 ProjectToPlane(vec3 point, vec3 planePoint, vec3 planeNormal)
{
    vec3 v = point - planePoint;
    float len = dot(v, planeNormal);
    vec3 d = len * planeNormal;
    return (point - d);
}

void CalculatePositions()
{
    // Original vertices stay the same, copy them over
    output_patch.FragPos_030 = vs_in[0].FragPos; 
    output_patch.FragPos_003 = vs_in[1].FragPos; 
    output_patch.FragPos_300 = vs_in[2].FragPos; 

    // Edges are opposite to their vertex
    vec3 edge_300 = output_patch.FragPos_003 - output_patch.FragPos_030;
    vec3 edge_030 = output_patch.FragPos_300 - output_patch.FragPos_003;
    vec3 edge_003 = output_patch.FragPos_030 - output_patch.FragPos_300;

    // Generate two midpoints for each edge
    output_patch.FragPos_021 = output_patch.FragPos_030 + edge_300 / 3.0;
    output_patch.FragPos_012 = output_patch.FragPos_030 + edge_300 * (2.0 / 3.0);

    output_patch.FragPos_102 = output_patch.FragPos_003 + edge_030 / 3.0;
    output_patch.FragPos_201 = output_patch.FragPos_003 + edge_030 * (2.0 / 3.0);

    output_patch.FragPos_210 = output_patch.FragPos_300 + edge_003 / 3.0;
    output_patch.FragPos_120 = output_patch.FragPos_300 + edge_003 * (2.0 / 3.0);

    // Project each midpoint onto plane defined by nearest vertex and its normal
    output_patch.FragPos_021 = ProjectToPlane(output_patch.FragPos_021, output_patch.FragPos_030, output_patch.Normal[0]);

    output_patch.FragPos_012 = ProjectToPlane(output_patch.FragPos_012, output_patch.FragPos_003, output_patch.Normal[1]);
    output_patch.FragPos_102 = ProjectToPlane(output_patch.FragPos_102, output_patch.FragPos_003, output_patch.Normal[1]);

    output_patch.FragPos_201 = ProjectToPlane(output_patch.FragPos_201, output_patch.FragPos_300, output_patch.Normal[2]);
    output_patch.FragPos_210 = ProjectToPlane(output_patch.FragPos_210, output_patch.FragPos_300, output_patch.Normal[2]);

    output_patch.FragPos_120 = ProjectToPlane(output_patch.FragPos_120, output_patch.FragPos_030, output_patch.Normal[0]);

    // Calculate center
    vec3 center = (output_patch.FragPos_003 + output_patch.FragPos_030 + output_patch.FragPos_300) / 3.0;
    output_patch.FragPos_111 = (output_patch.FragPos_021 + output_patch.FragPos_012 + 
                                output_patch.FragPos_102 + output_patch.FragPos_201 + 
                                output_patch.FragPos_210 + output_patch.FragPos_120) / 6.0;

    output_patch.FragPos_111 += (output_patch.FragPos_111 - center) / 2.0;
}

void main()
{
    // Set control points for output patch
    for(int i = 0; i < 3; i++)
    {
        output_patch.Normal[i] = vs_in[i].TangentNormal;
        output_patch.TexCoords[i] = vs_in[i].TexCoords;
    }

    CalculatePositions();

    // Copy input CPs into output CPs
    cs_out[gl_InvocationID].FragPos = vs_in[gl_InvocationID].FragPos;
    cs_out[gl_InvocationID].TexCoords = vs_in[gl_InvocationID].TexCoords;
    cs_out[gl_InvocationID].TangentLightPos = vs_in[gl_InvocationID].TangentLightPos;
    cs_out[gl_InvocationID].TangentViewPos = vs_in[gl_InvocationID].TangentViewPos;
    cs_out[gl_InvocationID].TangentFragPos = vs_in[gl_InvocationID].TangentFragPos;
    cs_out[gl_InvocationID].TangentNormal = vs_in[gl_InvocationID].TangentNormal;
    cs_out[gl_InvocationID].FragPosLightSpace = vs_in[gl_InvocationID].FragPosLightSpace;

	
//    
//	// Distance from Camera to the three control points
//	float cameraToVertexDistance0 = distance(cameraPos, cs_out[0].FragPos);
//	float cameraToVertexDistance1 = distance(cameraPos, cs_out[1].FragPos);
//	float cameraToVertexDistance2 = distance(cameraPos, cs_out[2].FragPos);
//
//	// Calculate Tesselation Levels
//	// TessLevelOuter ~ number of segments on each edge
//	gl_TessLevelOuter[0] = GetTessLevel(cameraToVertexDistance1, cameraToVertexDistance2);
//	gl_TessLevelOuter[1] = GetTessLevel(cameraToVertexDistance2, cameraToVertexDistance0);
//	gl_TessLevelOuter[2] = GetTessLevel(cameraToVertexDistance0, cameraToVertexDistance1);
//	// TessLevelInner ~ number of rings the triangles contain
//	gl_TessLevelInner[0] = gl_TessLevelOuter[2];

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = tesselationAmount;
    gl_TessLevelOuter[1] = tesselationAmount;
    gl_TessLevelOuter[2] = tesselationAmount;
    gl_TessLevelInner[0] = tesselationAmount;
}