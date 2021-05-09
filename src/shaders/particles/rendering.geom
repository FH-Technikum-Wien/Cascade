#version 460

// Treat vertices as points
layout(points) in;
// Return as triange strip
layout(triangle_strip) out;
// We are creating quads from a point -> 4 vertices
layout(max_vertices = 4) out;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform vec3 cameraPos;

uniform vec3 quad1;
uniform vec3 quad2;


in vec3 pColorPass[];
in float pLifetimePass[];
in float pSizePass[];
in float pTypePass[];

smooth out vec2 TexCoord;
flat out vec4 Color;


void main()
{
	if(pTypePass[0] != 0.0)
	{
		vec3 oldPos = gl_in[0].gl_Position.xyz;
		float size = pSizePass[0];
		mat4 viewProjMat = projectionMat * viewMat;
		
		// Fade out particles depending on lifetime
		Color = vec4(pColorPass[0], pLifetimePass[0]);

		// Generate first vertex
		vec3 pos = oldPos + (-quad1 - quad2) * size;
		TexCoord = vec2(0.0, 0.0);
		gl_Position = viewProjMat * vec4(pos, 1.0);
		EmitVertex();

		// Generate second vertex
		pos = oldPos + (-quad1 + quad2) * size;
		TexCoord = vec2(0.0, 1.0);
		gl_Position = viewProjMat * vec4(pos, 1.0);
		EmitVertex();

		// Generate third vertex
		pos = oldPos + (quad1 - quad2) * size;
		TexCoord = vec2(1.0, 0.0);
		gl_Position = viewProjMat * vec4(pos, 1.0);
		EmitVertex();

		// Generate third vertex
		pos = oldPos + (quad1 + quad2) * size;
		TexCoord = vec2(1.0, 1.0);
		gl_Position = viewProjMat * vec4(pos, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}





















