 #version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_color;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform vec3 droneCol;
//uniform vec3 color2;

out vec3 frag_color;

void main()
{
    gl_Position = Projection * View * Model * vec4(v_pos, 1.0);
    frag_color = droneCol;
}