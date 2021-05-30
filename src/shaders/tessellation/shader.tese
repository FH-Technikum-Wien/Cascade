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

in patch OUTPUT_PATCH output_patch;

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
    //es_out.FragPos = interpolate3D(cs_in[0].FragPos, cs_in[1].FragPos, cs_in[2].FragPos);
    es_out.TexCoords = interpolate2D(output_patch.TexCoords[0], output_patch.TexCoords[1], output_patch.TexCoords[2]);
    es_out.TangentNormal = interpolate3D(output_patch.Normal[0], output_patch.Normal[1], output_patch.Normal[2]);


    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);

    // Calculate position using the bezier triangle equation
    es_out.FragPos = output_patch.FragPos_300 * wPow3 +
                     output_patch.FragPos_030 * uPow3 +
                     output_patch.FragPos_003 * vPow3 +
                     output_patch.FragPos_210 * 3.0 * wPow2 * u +
                     output_patch.FragPos_120 * 3.0 * w * uPow2 +
                     output_patch.FragPos_201 * 3.0 * wPow2 * v +
                     output_patch.FragPos_021 * 3.0 * uPow2 * v +
                     output_patch.FragPos_102 * 3.0 * w * vPow2 +
                     output_patch.FragPos_012 * 3.0 * u * vPow2 +
                     output_patch.FragPos_111 * 6.0 * w * u * v;

    // Pass other data through
    es_out.TangentLightPos = cs_in[0].TangentLightPos;
    es_out.TangentViewPos = cs_in[0].TangentViewPos;
    es_out.TangentFragPos = cs_in[0].TangentFragPos;
    es_out.FragPosLightSpace = cs_in[0].FragPosLightSpace;

	float displacement = texture(displacementMap, es_out.TexCoords.xy).x;
    es_out.FragPos += es_out.TangentNormal * displacement * displacementFactor;
    
	gl_Position = projectionMat * viewMat * vec4(es_out.FragPos, 1.0);
}
