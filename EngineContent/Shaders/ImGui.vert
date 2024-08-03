#version 310 es

precision highp float;

layout(location=0) in vec2 vertexPosition;
layout(location=1) in vec2 vertexUV;
layout(location=2) in vec4 vertexColor;

layout(location=0) out vec2 fragUV;
layout(location=1) out vec4 fragColor;

layout(location=0) uniform mat4 projectionMatrix;

void main()
{
	fragUV = vertexUV;
	fragColor = vertexColor;
	gl_Position = projectionMatrix * vec4(vertexPosition, 0.0, 1.0);
}