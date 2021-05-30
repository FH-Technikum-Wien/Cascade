#version 460 core
// Position with vertex attribute position 0
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Texture
layout (location = 2) in vec2 aTexCoord;

// Local to World
uniform mat4 modelMat;

out vec3 FragPos_VS;
out vec2 TexCoord_VS;
out vec3 Normal_VS;

void main()
{
    FragPos_VS = vec3(modelMat * vec4(aPos, 1.0));
    Normal_VS = vec3(modelMat * vec4(aNormal, 1.0));
    TexCoord_VS = aTexCoord;
}