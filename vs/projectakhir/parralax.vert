#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform float n = 1.0f;
uniform mat4 model;
uniform mat4 projection;
uniform int flip = 0;
uniform float x;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = projection * model * vec4(position, 1.0f);
	ourColor = color;
    TexCoord = vec2(texCoord.x + 0.002f * x, texCoord.y);
}