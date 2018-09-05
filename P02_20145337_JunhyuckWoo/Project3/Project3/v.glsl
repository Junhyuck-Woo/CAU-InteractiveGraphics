#version 400

layout (location = 0) in vec3 vPosition;

uniform mat4 m_matrix;
uniform mat4 proj_matrix;
uniform vec3 color;

void main()
{
    gl_Position = proj_matrix * m_matrix * vec4(vPosition, 1.0);
}