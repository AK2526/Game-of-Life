#include <cells.h>
#include <iostream>

int wCells;
int hCells;

void initialize()
{
	try
	{
		std::cout << "Please enter the width of the grid (units)" << std::endl;
		std::cin >> wCells;
		if (wCells < 1)
			throw std::exception("Invalid input");

		std::cout << "Please enter the height of the grid (units)" << std::endl;

		std::cin >> hCells;
		if (hCells < 1)
			throw std::exception("Invalid input");
	}
	catch (const std::exception&)
	{
		wCells = 100;
		hCells = 100;
	}


}