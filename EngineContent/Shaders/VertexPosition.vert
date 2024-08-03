#version 310 es

precision highp float;

layout(location=0) in highp vec3 vertexPosition;

layout(location=0) uniform highp mat4 projectionMatrix;
layout(location=1) uniform highp mat4 viewMatrix;
layout(location=2) uniform highp mat4 worldMatrix;

void main()
{
	mat4 modelMatrix = projectionMatrix * viewMatrix * worldMatrix;
	gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}