#version 310 es

precision highp float;

layout(location=0) in vec3 appPosition;
layout(location=1) in vec3 appNormal;

layout(location=0) out vec3 vertNormal;

layout(location=0) uniform mat4 projection;
layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 world;

void main()
{
	vertNormal = appNormal;
	gl_Position = projection * view * world * vec4(appPosition, 1.0f);
}