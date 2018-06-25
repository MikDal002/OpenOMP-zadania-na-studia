// ProjektPragmaZad5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ratio>

double TheFunction(double x)
{
	if (x >= 0 && x <= 2)
	{
		return 3.5 * x;
	}
	if (x > 2 && x <= 5)
	{
		return (-(7.0 / 3.0)*x) + (35.0 / 3.0);
	}
	if (x > 5 && x <= 7)
	{
		return 2.0 * x - 10.0;
	}
	if (x > 7 && x <= 9)
	{
		return -2.0 * x + 18.0;
	}
	return NAN;
}

void DrawChart(std::function<double(double)> functionToDraw, double start, double end)
{
	const int MAX_WIDTH = 200;
	const int MAX_HEIGHT = 60;

	std::vector<double> functionValues = std::vector<double>(MAX_WIDTH);

	double xPerWidth = (end - start) / MAX_WIDTH;
	

	for (int i = 0; i < MAX_WIDTH; i++)
		functionValues[i] = functionToDraw(start + xPerWidth * i);
	
	double max = *std::max_element(functionValues.begin(), functionValues.end());
	double min = *std::min_element(functionValues.begin(), functionValues.end());

	double heightPerY = MAX_HEIGHT / (max - min);

	for (int row = 0; row <= MAX_HEIGHT ; row++)
	{
		int space = 0;
		for (int column = 0; column < MAX_WIDTH; column++)
		{
			if (MAX_HEIGHT - row == abs(round(min * heightPerY))) // kikedy zero na wykresie
			{
				std::cout << "-";
			}
			else if (round((functionValues[column] - min) * heightPerY) == (MAX_HEIGHT - row))
			{
				for(space += 1 ; space < column; ++space)
				{
					std::cout << " ";
				}
				std::cout << "*";
			}
		}
		std::cout << std::endl;
	}
}

double IntegralByRectangleRule (std::function<double(double)> functionToIntegrate, double start, double stop, int steps = 100)
{
	double stepSize = (stop - start) / steps;
	double sum = 0;
	for (int i = 0; i < steps; i++)
	{
		double x = start + i * stepSize;
		double y = functionToIntegrate(x);
		sum += y;
	}
	sum *= stepSize;

	return sum;
}


double CalcFourierElementAn(int number, double start, double stop)
{
	std::function<double(double)> anElement = [&number](double x) { return TheFunction(x)* cos(number) * x; };
	return IntegralByRectangleRule(anElement, start, stop) / stop;
}

double CalcFourierElementBn(int number, double start, double stop)
{
	std::function<double(double)> bnElement = [&number](double x) { return TheFunction(x)* sin(number) * x; };
	return IntegralByRectangleRule(bnElement, start, stop) / stop;
}

int main()
{
	DrawChart(TheFunction, 0, 9);

	getchar();
	return 0;
}

