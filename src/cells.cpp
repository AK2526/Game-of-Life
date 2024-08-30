#include <cells.h>
#include <glad/glad.h>

int getCellInd(int x, int y)
{
	return x * hCells + y % hCells;
}

bool inGrid(int x, int y)
{
	if (0 <= x && x < wCells && 0 <= y && y < hCells)
	{
		return true;
	}
	return false;
}

void setCellCol(int x, int y, int col)
{
	int i = getCellInd(x, y) * 16;
	vertices[i + 2] = col;
	vertices[i + 6] = col;
	vertices[i + 10] = col;
	vertices[i + 14] = col;


	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(int), sizeof(int) * 16, &vertices[i]);
}


void updateCol(int i, int col)
{
	vertices[i] = col;
	vertices[i + 4] = col;
	vertices[i + 8] = col;
	vertices[i + 12] = col;
}


void setCellStat(int x, int y, int col)
{

	int i = getCellInd(x, y) * 16;

	vertices[i + 3] = col;
	vertices[i + 7] = col;
	vertices[i + 11] = col;
	vertices[i + 15] = col;

	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(int), sizeof(int) * 16, &vertices[i]);
}

void addPoint(std::vector<int>& v, int x1, int y1, int stat)
{
	v.push_back(x1);
	v.push_back(y1);
	v.push_back(stat);
	v.push_back(IDLE);
}
