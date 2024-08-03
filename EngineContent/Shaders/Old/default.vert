#version 310 es

precision highp float;

layout(location=0) in vec3 appPosition;
layout(location=1) in vec3 appNormal;
layout(location=2) in vec2 appUV;

layout(location=0) out vec3 vertNormal;
layout(location=1) out vec2 vertUV;

layout(location=0) uniform mat4 projection;
layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 world;

void main()
{
	vertNormal = appNormal;
	vertUV = appUV;
	gl_Position = projection * view * world * vec4(appPosition, 1.0f);
}