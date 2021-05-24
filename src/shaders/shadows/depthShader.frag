#version 330 core
// No color buffer and disabled draw and read buffers 
// -> No need for fragment shader 
// -> Leave it empty

void main()
{
	// This happens by default, not needed:
	// gl_FragDepth = gl_FragCoord.z;
}