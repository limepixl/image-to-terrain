#version 460 core
in float y;
in vec4 sampled;

out vec4 color;

void main() 
{
	color = vec4(1.0, 1.0, 1.0, 1.0) * y;
}