#pragma once
#include <vector>

extern int wCells;
extern int hCells;

const int DYING = 1, DEAD = 2, BORN = 3, ALIVE = 4;
const int IDLE = 0, HOVER = 1;

extern std::vector<int> cellNeighbours;
extern std::vector<int> vertices;

void updateNeighboursSimple(int index);
void updateNeighboursComplex(int index);
void updateCells();

int getCellInd(int x, int y);
bool inGrid(int x, int y);
void setCellCol(int x, int y, int col);
void updateCol(int i, int col);
void setCellStat(int x, int y, int col);
void addPoint(std::vector<int>& v, int x1, int y1, int stat);