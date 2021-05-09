#version 330 core
// Position with vertex attribute position 0
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Texture
layout (location = 2) in vec2 aTexCoord;
// Tangents for normal mapping
layout (location = 3) in vec3 aTangent;


out VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

// Local to World
uniform mat4 modelMat;
// World to View (Camera)
uniform mat4 viewMat;
// View to Clip (perspective)
uniform mat4 projectionMat;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
    vs_out.FragPos = vec3(modelMat * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoord;

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

    gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}