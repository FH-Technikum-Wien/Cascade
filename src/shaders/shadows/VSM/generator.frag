#version 330 core


void main()
{
	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	// Prevent shadow acne at steep angles
	float depthSquareWithBias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, depthSquareWithBias, 0.0, 0.0);
}