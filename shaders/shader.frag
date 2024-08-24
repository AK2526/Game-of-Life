#version 330 core
in vec2 texCoord;
in float stats;
in vec4  col;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
	FragColor = mix(texture(texture1, texCoord), col, 0.9) ;

};