#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

in vec2 TexCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;

uniform vec3 textureColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float focus;


uniform vec3 lightColor;
uniform float lightIntensity;
uniform float ambientLightAmount;

uniform float bumpiness;

void main()
{
    // Get normal from normal map [0,1] and tranform to tangent space [-1,1]
    vec3 normal = normalize(texture(normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0);
    normal.xy *= bumpiness;
    normal = normalize(normal);

    vec3 lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 cameraDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + cameraDirection);

    // Main texture color
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb * textureColor;

    // Ambient Light
    vec3 ambientLight = (ambientStrength * ambientLightAmount) * color;

    // Diffuse Light
    vec3 diffuseLight = (max(0.0, dot(lightDirection, normal)) * diffuseStrength) * color;

    // SpecularLight
    vec3 specularLight = (pow(max(0.0, dot(normal, halfwayDirection)), focus) * specularStrength) * lightIntensity * lightColor;
    

    vec3 lighting = (diffuseLight + specularLight + ambientLight) * color;
    FragColor = vec4(lighting, 1.0);
}
