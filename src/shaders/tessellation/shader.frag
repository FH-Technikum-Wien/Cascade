#version 460 core
out vec4 FragColor;

in ES_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} es_in;

in vec2 TexCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;

uniform vec3 textureColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float focus;


uniform vec3 lightColor;
uniform float lightIntensity;
uniform float ambientLightAmount;

uniform float bumpiness;

float linearStep(float low, float high, float value)
{
    return clamp((value - low) / (high - low), 0.0, 1.0);
}


void main()
{
    vec3 cameraDirection = normalize(es_in.TangentViewPos - es_in.TangentFragPos);
    // Offset coords with ParallaxMapping
    vec2 texCoords = es_in.TexCoords;

    // If outside of [0,1], discard
    //if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    //   discard;

    vec3 normal = normalize(es_in.TangentNormal);
    vec3 lightDirection = normalize(es_in.TangentLightPos - es_in.TangentFragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + cameraDirection);

    // Get normal from normal map [0,1] and tranform to tangent space [-1,1]
    normal = normalize(texture(normalMap, texCoords).rgb * 2.0 - 1.0);
    normal.xy *= bumpiness;
    normal = normalize(normal);

    // Main texture color
    vec3 color = texture(diffuseTexture, texCoords).rgb * textureColor;

    // Ambient Light
    vec3 ambientLight = (ambientStrength * ambientLightAmount) * color;

    // Diffuse Light
    vec3 diffuseLight = (max(0.0, dot(lightDirection, normal)) * diffuseStrength) * color;

    // SpecularLight
    vec3 specularLight = (pow(max(0.0, dot(normal, halfwayDirection)), focus) * specularStrength) * lightIntensity * lightColor;

    vec3 lighting = diffuseLight + specularLight + ambientLight;
    FragColor = vec4(lighting, 1.0);
}
