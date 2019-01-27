#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexPos;

out float y;
out vec2 texPos;
out vec4 sampled;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;
uniform sampler2D tex;

void main() 
{
	sampled = texture(tex, aTexPos);
	y = sampled.r;

	gl_Position = projection * view * model * vec4(aPos.x, y * scale, aPos.z, 1.0);
}