#version 430

in vec3 pos;
uniform float chunkHeight;

out float noise;

const float radius = 0.2;
const float radius2 = 0.2;

void main()
{   
    // Move center depending on height. From -0.25 to 0.25
    float centerX = cos(pos.y) * 0.5;
    float centerX2 = -cos(pos.y) * 0.5;
    // Move z coord to middle 
    float centerZ = sin(pos.y) * 0.25+ 0.5;
    float centerZ2 = -sin(pos.y) * 0.25 + 0.5;
    // Alter radius depending on height
    float newRadius = radius + ((radius / 4) * cos(pos.y));
    float newRadius2 = radius2 + ((radius2 / 4) * cos(pos.y));

    // Check whether current point lies within a circle
    noise = int(pow(pos.x - centerX, 2) + pow(pos.z - centerZ, 2) < pow(newRadius, 2) || 
                pow(pos.x - centerX2, 2) + pow(pos.z - centerZ2, 2) < pow(newRadius2, 2));
    
}