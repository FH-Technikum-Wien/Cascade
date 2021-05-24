#version 460 core
uniform sampler2D filterTexture;

in vec2 TexCoord;

uniform vec3 blurScale;

void main()
{
    vec4 color = vec4(0.0);
    // 7x1 blur
    color += texture(filterTexture, TexCoord + (vec2(-3.0) * blurScale.xy)) * (1.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2(-2.0) * blurScale.xy)) * (6.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2(-1.0) * blurScale.xy)) * (15.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2( 0.0) * blurScale.xy)) * (20.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2( 1.0) * blurScale.xy)) * (15.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2( 2.0) * blurScale.xy)) * (6.0/64.0);
    color += texture(filterTexture, TexCoord + (vec2( 3.0) * blurScale.xy)) * (1.0/64.0);

    gl_FragColor = color;
}
