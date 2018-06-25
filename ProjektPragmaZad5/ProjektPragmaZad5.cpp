// ProjektPragmaZad5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ratio>
#define _USE_MATH_DEFINES
#include <math.h>
#include <omp.h>
#include <ctime>

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
	const int MAX_WIDTH = 300;
	const int MAX_HEIGHT = 30;

	std::vector<double> functionValues = std::vector<double>(MAX_WIDTH);

	double xPerWidth = (end - start) / MAX_WIDTH;

#	pragma omp parallel for
	for (int i = 0; i < MAX_WIDTH; i++)
		functionValues[i] = functionToDraw(start + xPerWidth * i);

	double max = *std::max_element(functionValues.begin(), functionValues.end());
	double min = *std::min_element(functionValues.begin(), functionValues.end());

	double heightPerY = MAX_HEIGHT / (max - min);

	if (heightPerY == INFINITY) heightPerY = 1;

	for (int row = 0; row <= MAX_HEIGHT; row++)
	{
		int space = 0;
		for (int column = 0; column < MAX_WIDTH; column++)
		{
			if (MAX_HEIGHT - row == abs(round(min * heightPerY))) // kiedy zero na wykresie
			{
				std::cout << "-";
			}
			else if (round((functionValues[column] - min) * heightPerY) == (MAX_HEIGHT - row))
			{
				for (space += 1; space < column; ++space)
				{
					std::cout << " ";
				}
				std::cout << "*";
			}
		}
		std::cout << std::endl;
	}
}

double IntegralByRectangleRule(std::function<double(double)> functionToIntegrate, double start, double stop, int steps = 100)
{
	double stepSize = (stop - start) / steps;
	double sum = 0;

#	pragma omp parallel for reduction(+: sum)
	for (int i = 0; i < steps; i++)
	{
		double x = start + i * stepSize;
		double y = functionToIntegrate(x);
		sum += y;
	}
	sum *= stepSize;

	return sum;
}


double CalcFourierElementAn(std::function<double(double)> function, int number, double start, double stop)
{
	std::function<double(double)> anElement = [=](double x) { return function(x)* cos(2 * number * M_PI * x / (stop - start)); };
	return 2 * IntegralByRectangleRule(anElement, start, stop) / (stop - start);
}

double CalcFourierElementBn(std::function<double(double)> function, int number, double start, double stop)
{
	std::function<double(double)> bnElement = [=](double x) { return function(x)* sin(2 * number * M_PI * x / (stop - start)); };
	return 2 * IntegralByRectangleRule(bnElement, start, stop) / (stop - start);
}

std::function<double(double)> CreateTrigoFourierSeries(std::function<double(double)> functionToCast, double start, double stop, int steps = 5)
{
	std::function<double(double, double, double, double)> singleStep = [=](double step, double x, double start, double stop)
	{
		double an = CalcFourierElementAn(functionToCast, step, start, stop);
		double bn = CalcFourierElementBn(functionToCast, step, start, stop);
		double anCos = cos(2.0 * step * M_PI * x / (stop - start));
		double bnSin = sin(2.0 * step * M_PI * x / (stop - start));

		return an * anCos + bn * bnSin;
	};

	return [=](double x)
	{
		double a0 = CalcFourierElementAn(functionToCast, 0, start, stop);
		double sum = a0/2;

#		pragma omp parallel for reduction (+:sum)
		for (int i = 1; i < steps; i++)
		{
			sum += singleStep(i, x, start, stop);
		}
		return sum;
	};
}
double JustInvokeFunction(std::function<double(double)> function, double start, double stop, int steps)
{
	double stepSize = (stop - start) / (double)steps;
	double retValue = 0;
	for (int i = 0; i < steps; i++)
	{
		retValue += function(start + i * stepSize);
	}
	return retValue;
}
int main()
{
	DrawChart(TheFunction, 0, 9);
	DrawChart(CreateTrigoFourierSeries(TheFunction, 0, 9, 2), 0, 9);
	DrawChart(CreateTrigoFourierSeries(TheFunction, 0, 9, 5), 0, 9);
	DrawChart(CreateTrigoFourierSeries(TheFunction, 0, 9, 20), 0, 9);

	int startElementsInSeries = 100;
	int endElementsInSeries = 110;

	clock_t parallelTime = clock();
	omp_set_num_threads(omp_get_max_threads());

	for (int i = startElementsInSeries; i < endElementsInSeries; i += 1)
	{
		auto fourierSeries = CreateTrigoFourierSeries(TheFunction, 0, 9, i);
		JustInvokeFunction(fourierSeries, 0, 9, 100);
	}
	parallelTime = clock() - parallelTime;

	clock_t serialTime = clock();
	omp_set_num_threads(1);

	for (int i = startElementsInSeries; i < endElementsInSeries; i += 1)
	{
		auto fourierSeries = CreateTrigoFourierSeries(TheFunction, 0, 9, i);
		JustInvokeFunction(fourierSeries, 0, 9, 100);
	}
	serialTime = clock() - serialTime;;

	std::cout << "Czas rownolegly: " << (double)(parallelTime) / CLOCKS_PER_SEC << "s, czas szeregowy: " << (double)(serialTime) / CLOCKS_PER_SEC << "s." << std::endl;
	getchar();
	return 0;
}

