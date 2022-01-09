#version 460 core
layout (location = 0) in vec4 position;
//out vec2 coord;
//varying vec3 vertex_color;

void main()
{
    gl_Position = position;
	//gl_Position = ftransform();
	//vertex_color = gl_Vertex.xyz; 
	// Example of how to use swizzling.   
	// This is the same as vertex_color = vec3(gl_Vertex);

	// send the uv coord to the next stage
	//coord = vec2(gl_TexCoord);
}