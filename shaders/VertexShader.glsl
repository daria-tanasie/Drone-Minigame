 #version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_color;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform float frecv;
uniform vec3 color1;
uniform vec3 color2;

out vec3 frag_color;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f*f*(3.0-2.0*f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
	vec2 xzPosition = vec2(v_pos.x, v_pos.z) * frecv;
    float newPos = noise(xzPosition);
    gl_Position = Projection * View * Model * vec4(v_pos.x,newPos , v_pos.z, 1.0);
    frag_color = mix(color2, color1, newPos);
}