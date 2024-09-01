#version 310 es

precision highp float;

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexUV;

layout(location=0) out vec3 fragNormal;
layout(location=1) out vec2 fragUV;

layout(location=0) uniform mat4 projectionMatrix;
layout(location=1) uniform mat4 viewMatrix;
layout(location=2) uniform mat4 worldMatrix;

void main()
{
    fragNormal = vertexNormal;
    fragUV = vec2(vertexUV.x, 1.0 - vertexUV.y);

    mat4 modelMatrix = projectionMatrix * viewMatrix * worldMatrix;
    gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}