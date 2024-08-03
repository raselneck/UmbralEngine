#version 310 es

precision highp float;

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec4 vertexColor;

layout(location=0) out vec4 fragColor;

layout(location=0) uniform mat4 projectionMatrix;
layout(location=1) uniform mat4 viewMatrix;
layout(location=2) uniform mat4 worldMatrix;

void main()
{
	fragColor = vertexColor;

	mat4 modelMatrix = projectionMatrix * viewMatrix * worldMatrix;
	gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}