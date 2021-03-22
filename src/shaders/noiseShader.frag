#version 430

in vec3 pos;
uniform float chunkHeight;

out float noise;

const float radius = 0.2;

void main()
{
    /*
    float sinX = sin(pos.x * 5.8905);
    float cosY = cos(pos.y * 5.8905);
    float cosZ = cos(pos.z * 5.8905);
    //noise = (sinX * sinX + cosY * cosY + cosZ * cosZ) * (1.0f / 3.0f);
    noise = 1;
    */
    
    // Move center depending on height. From -0.5 to 0.5
    float center_x = cos(pos.y) * 0.5;
    // Alter radius depending on height
    float newRadius = radius + ((radius / 2) * cos(pos.y));

    //noise = 1;
    noise = int(pow(pos.x - center_x, 2) + pow(pos.z, 2) < pow(newRadius, 2));
    
}