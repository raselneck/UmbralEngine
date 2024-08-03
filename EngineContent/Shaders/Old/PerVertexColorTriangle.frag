#version 310 es

precision highp float;

layout(location = 0) in vec3 vertexColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(vertexColor, 1.0);
}