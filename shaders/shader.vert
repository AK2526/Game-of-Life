#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aCol;
layout (location = 2) in float aStat;

uniform float offset;
uniform mat4 final;

out vec2 texCoord;
out vec4 col;

const int DYING = 1, DEAD = 2, BORN = 3, ALIVE = 4;

const int IDLE = 0, HOVER = 1;
void main()
{
// 

	gl_Position = final * vec4(aPos.x, aPos.y, 0.0f, 1.0);

	if (aCol == DEAD)
	{
		col = vec4(0.1, 0.02, 0.01, 1.0);
	}
	if (aCol == DYING)
	{
		col = vec4(0.2, 0.0, 0.0, 1.0);
	}
	if (aCol == BORN)
	{
		col = vec4(0.5, 0.0, 1.0, 1.0);
	}
	if (aCol == ALIVE )
	{
		col = vec4(0.0, 0.0, 1.0, 1.0);
	}
	if (aStat == HOVER)
	{
		col += vec4(0.3, 0.3, 0.3, 0.0);
	}


	texCoord = aPos;
};