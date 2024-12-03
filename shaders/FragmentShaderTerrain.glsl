#version 330

in vec3 col1;
in vec3 col2;
in float noise_col;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 frag_color = mix(col2, col1, noise_col);

	out_color = vec4(frag_color, 1);
}
