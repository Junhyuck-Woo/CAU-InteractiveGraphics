#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec4 vTangent;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 Position;
out vec3 Normal;
out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

void main()
{
	Position = vec3(mv_matrix*vec4(vPosition, 1.0));
	Normal = normalize(mat3(mv_matrix)*vNormal);
	vec3 tang = normalize(mat3(mv_matrix) * vTangent.xyz);
	vec3 binormal = cross(Normal, tang);
	vec3 L = Light.Position.xyz - Position;
	LightDir = vec3(dot(L, tang), dot(L, binormal), dot(L, Normal));
	ViewDir = vec3(dot(-Position, tang), dot(-Position, binormal), dot(-Position, Normal));
	TexCoord = vTexCoord;
	gl_Position = proj_matrix*mv_matrix*vec4(vPosition,1.0);
}