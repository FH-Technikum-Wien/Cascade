#version 460

// Treat vertices as points
layout(points) in;
// Return as points
layout(points) out;
// Limit emitted vertices to 85 (GTX 1070 TI limit)
layout(max_vertices = 40) out;

// Input from vertex shader
in vec3 pPositionPass[];
in vec3 pVelocityPass[];
in vec3 pColorPass[];
in float pLifetimePass[];
in float pSizePass[];
in float pTypePass[];

// Output
out vec3 pPositionOut;
out vec3 pVelocityOut;
out vec3 pColorOut;
out float pLifetimeOut;
out float pSizeOut;
out float pTypeOut;

// Generation values
uniform vec3 gPosition;
uniform vec3 gVelocityMin;
uniform vec3 gVelocityRange;
uniform vec3 gGravity;
uniform vec3 gColor;
uniform float gSize;
uniform float gLifetimeMin;
uniform float gLifetimeRange;
uniform int gNumberOfParticlesToSpawn;
uniform vec3 gRandomSeed;

// System time that has passed
uniform float sTimePassed;

vec3 seed;

/* 
* Returns a random number between 0 and 1
* See: https://www.mbsoftworks.sk/tutorials/opengl3/23-particle-system/
*/
float random01()
{
	uint n = floatBitsToUint(seed.y * 214013.0 + seed.x * 2531011.0 + seed.z * 141251.0);
	n = n * (n * n * 15731u + 789221u);
	n = (n >> 9u) | 0x3F800000u;

	float fRes =  2.0 - uintBitsToFloat(n);
    seed = vec3(seed.x + 147158.0 * fRes, seed.y * fRes  + 415161.0 * fRes, seed.z + 324154.0 * fRes);
    return fRes;
}


void main()
{
	seed = gRandomSeed;

	// Update values
	pPositionOut = pPositionPass[0];
	pVelocityOut = pVelocityPass[0];
	pColorOut = pColorPass[0];
	// Update life time
	pLifetimeOut = pLifetimePass[0] - sTimePassed;
	pSizeOut = pSizePass[0];
	pTypeOut = pTypePass[0];

	// Apply physics if it's a normal particle
	if(pTypeOut != 0.0) pPositionOut += pVelocityOut * sTimePassed;
	if(pTypeOut != 0.0) pVelocityOut += gGravity * sTimePassed;

	if(pTypeOut == 0.0)
	{
		// Always emit generator particle
		EmitVertex();
		EndPrimitive();

		for(int i = 0; i < gNumberOfParticlesToSpawn; i++)
		{
			pPositionOut = gPosition;
			pColorOut = gColor;
			pSizeOut = gSize;
			// Calculate random values in defined range
			pVelocityOut = gVelocityMin + vec3(
					gVelocityRange.x * random01(), 
					gVelocityRange.y * random01(), 
					gVelocityRange.z * random01()
			);

			pLifetimeOut = gLifetimeMin + gLifetimeRange * random01();
			// Define as normal particle
			pTypeOut = 1.0;

			// Emit new particle
			EmitVertex();
			EndPrimitive();
		}
	}
	else if(pLifetimeOut > 0.0)
	{
		// If particle has life time remaining, emit it
		EmitVertex();
		EndPrimitive();
	}

}





















