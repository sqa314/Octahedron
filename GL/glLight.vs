#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model* vec4(1.0f,1.0f,1.0f,1.0f);
}