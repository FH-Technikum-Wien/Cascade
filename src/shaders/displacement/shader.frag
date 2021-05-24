#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
    vec4 FragPosLightSpace;
} fs_in;

in vec2 TexCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;
uniform sampler2D shadowMap;

uniform vec3 textureColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float focus;


uniform vec3 lightColor;
uniform float lightIntensity;
uniform float ambientLightAmount;

uniform float bumpiness;
uniform float heightScale;

uniform int steps;
uniform int refinementSteps;

float calculateShadowAmount(vec4 fragPosLightSpace, vec3 lightDirection, vec3 normal){
    // Transform to range between -1 and 1.
    vec3 projectionCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Tranform to range between 0 and 1 for depthMap.
    projectionCoords = projectionCoords * 0.5 + 0.5;
    
    // If outside of far plane -> no shadow
    if(projectionCoords.z > 1.0)
        return 0.0;

    // Get closest depth from light's perspective.
    float closestDepth = texture(shadowMap, projectionCoords.xy).r;
    // Get closest depth from camera's perspective.
    float currentDepth = projectionCoords.z;

    // Define bias to prevent shadow acne.
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);  
    // Check if current is bigger and thereby in shadow.
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 cameraDirection)
{
    // Scale cameraDirection vector
    vec2 p = cameraDirection.xy / cameraDirection.z * heightScale;

    // Last data which is still above displaced surface
    vec2 lastTexCoords = texCoords;
    float lastDepth = 0.0;

    vec2 currentTexCoords = texCoords;

    float stepSize;
    vec2 deltaTexCoords;
    float currentDisplacementDepth;
    float currentDepth;

    // Repeat for each refinement step
    for(int i = 1; i <= refinementSteps; ++i)
    {
        // Decrease stepSize depending on refinement depth (10 -> 100 -> 1000 -> ...)
        float stepAmount = pow(steps, i);
        // Shift amount per step
        deltaTexCoords = p / stepAmount;
        // Step size
        stepSize = 1.0 / stepAmount;
        
        // Continue from last depth (which is the last depth above the displaced surface)
        currentTexCoords = lastTexCoords;
        currentDepth = lastDepth;
        currentDisplacementDepth = texture(displacementMap, lastTexCoords).r;

        // Find first depth that is below displaced surface
        while(currentDepth < currentDisplacementDepth)
        {
            // Save data for next refinement (before below displaced surface)
            lastTexCoords = currentTexCoords;
            lastDepth = currentDepth;

            // Shift texCoords along direction of p
            currentTexCoords -= deltaTexCoords;
            // Get displacement value for it
            currentDisplacementDepth = texture(displacementMap, currentTexCoords).r;
            // Get next depth
            currentDepth += stepSize;
        }
    }

    return currentTexCoords;
}


void main()
{
    vec3 cameraDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    // Offset coords with ParallaxMapping
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords, cameraDirection);

    // If outside of [0,1], discard
    //if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    //    discard;

    vec3 normal = normalize(fs_in.TangentNormal);
    vec3 lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + cameraDirection);

    // Shadows
    float shadowAmount = calculateShadowAmount(fs_in.FragPosLightSpace, lightDirection, normal);

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

    

    vec3 lighting = (1.0 - shadowAmount) * (diffuseLight + specularLight) + ambientLight;
    FragColor = vec4(lighting, 1.0);
}
