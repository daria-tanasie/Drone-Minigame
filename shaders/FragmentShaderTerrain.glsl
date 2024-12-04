#version 330

in float noise_col;

uniform vec3 color1;
uniform vec3 color2;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 frag_color = mix(color2, color1, noise_col);
	out_color = vec4(frag_color, 1);
}
