#version 330

in vec3 frag_color;

uniform vec3 colorObs;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(colorObs, 1);
}