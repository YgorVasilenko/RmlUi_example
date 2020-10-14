#version 330 core
in vec2 TexCoord;
in vec4 ourColor;
uniform sampler2D ourTexture;
out vec4 FragColor;
void main()
{
	FragColor = texture(ourTexture, TexCoord);
	FragColor = FragColor * ourColor;
};