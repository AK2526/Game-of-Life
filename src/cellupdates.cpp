#include <cells.h>
#include <glad/glad.h>

void updateNeighboursSimple(int index)
{
	cellNeighbours[index - 1]++;
	cellNeighbours[index + 1]++;

	index = index - 1 - hCells;
	cellNeighbours[index++]++;
	cellNeighbours[index++]++;
	// cellNeighbours[index]; cuz i dont remmeber if i need this

	index += hCells + hCells;
	cellNeighbours[index--]++;
	cellNeighbours[index--]++;
	cellNeighbours[index]++;

}

void updateNeighboursComplex(int index)
{
	int x = index / hCells;
	int y = index % hCells;

	int left = (x - 1 + wCells) % wCells;
	int right = (x + 1 + wCells) % wCells;
	int up = (y + 1 + hCells) % hCells;
	int down = (y - 1 + hCells) % hCells;

	cellNeighbours[getCellInd(left, up)]++;
	cellNeighbours[getCellInd(x, up)]++;
	cellNeighbours[getCellInd(right, up)]++;
	cellNeighbours[getCellInd(left, y)]++;
	cellNeighbours[getCellInd(right, y)]++;
	cellNeighbours[getCellInd(left, down)]++;
	cellNeighbours[getCellInd(x, down)]++;
	cellNeighbours[getCellInd(right, down)]++;

}

void updateCells()
{

	cellNeighbours = std::vector<int>(wCells * hCells, 0);

	int i = 2;
	int j;

	for (j = 0; j < hCells; j++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		i += 16;
	}

	for (int k = 1; k < wCells - 1; k++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		j++;
		i += 16;
		for (int l = 1; l < hCells - 1; l++)
		{
			if (vertices[i] == ALIVE || vertices[i] == BORN)
				updateNeighboursSimple(j);
			j++;
			i += 16;
		}
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		j++;
		i += 16;
	}
	for (int k = 0; k < hCells; k++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		j++;
		i += 16;
	}



	j = -1;

	for (int i = 2; i < vertices.size(); i += 16)
	{

		j++;
		// IF you are a dead cell
		if ((vertices[i] == DEAD || vertices[i] == DYING) && cellNeighbours[j] == 3)
		{
			updateCol(i, BORN);
			continue;
		}
		else if (vertices[i] == DYING)
		{
			updateCol(i, DEAD);
			continue;
		}

		if ((vertices[i] == ALIVE || vertices[i] == BORN) && (cellNeighbours[j] < 2 || cellNeighbours[j] > 3))
		{
			updateCol(i, DYING);
			continue;
		}
		else if (vertices[i] == BORN)
			updateCol(i, ALIVE);

	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(int), &vertices[0]);

}
