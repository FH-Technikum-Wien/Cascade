#version 460 core
// Position with vertex attribute position 0
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Texture
layout (location = 2) in vec2 aTexCoord;
// Tangents for normal mapping
layout (location = 3) in vec3 aTangent;

// Local to World
uniform mat4 modelMat;

out VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} vs_out;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform mat4 lightSpaceMat;

void main()
{
    vec3 fragPos = vec3(modelMat * vec4(aPos, 1.0));

    vs_out.FragPos = fragPos;
    
    vs_out.TexCoords = aTexCoord;
    vs_out.FragPosLightSpace = lightSpaceMat * vec4(fragPos, 1.0);

    // Create TBN-Vector (Tangent Bitangent Normal)
    mat3 normalMatrix = transpose(inverse(mat3(modelMat)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    // Create Bitangent with cross of T and N
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * cameraPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.TangentNormal = TBN * aNormal;
}