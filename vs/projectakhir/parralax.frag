#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

// Texture samplers
uniform sampler2D ourTexture;

void main()
{
	vec4 bg = texture(ourTexture, TexCoord);
    gl_FragColor = bg * vec4(ourColor, 1.0f);
}