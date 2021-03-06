// ProjektPragmaZad2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <ctime>
#define _USE_MATH_DEFINES // for C++  
#include <math.h>
#include <omp.h>
#include <functional>

using namespace std;


class CalkaPodwojna {
private:
	double a, c;
	double b, d;
	double e;

protected:
	std::function<double(double x, double y)> f;

public:
	CalkaPodwojna(std::function<double(double x, double y)> f, double a, double b, double c, double d, double e) {
		this->f = f;
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->e = e;
	}

	//bez zrównoleglenia

	double licz() {
		double wynik = 0.0;
		const int N1 = abs(b - a) / e;
		const int N2 = abs(d - c) / e;


		for (int i = 1; i <= N1; i++) {
			double x = a + i * e;

			
			for (int j = 1; j < N2; j++) {
				double y = c+ j * e;

				wynik += f(x, y) * e;
			}
		}

		return wynik * e;
	}



	//zrównoleglenie 1 (Najszybsze)

	double licz1() {
		double wynik = 0.0;
		const int N1 = abs(b - a) / e;
		const int N2 = abs(d - c) / e;


#pragma omp parallel for default(none) shared(N1, N2)
		for (int i = 1; i <= N1; i++) {
			double x = a + i * e;

#pragma omp parallel for default(none) reduction(+:wynik)
			for (int j = 1; j < N2; j++) {
				double y = c+ j * e;
#pragma omp atomic
				wynik += f(x, y) * e;
			}
		}

		return wynik * e;
	}

	//zrównoleglenie 2

	double licz2() {
		double wynik = 0.0;
		const int N1 = abs(b - a) / e;
		const int N2 = abs(d - c) / e;

#pragma omp parallel for default(none) shared(N1, N2)
		for (int i = 1; i <= N1; i++) {
			double x = a+i * e;

#pragma omp parallel for default(none) 
			for (int j = 1; j < N2; j++) {
				double y = c+ j * e;
#pragma omp atomic
				wynik += f(x, y) * e;
			}
		}

		return wynik * e;
	}




//zrównoleglenie 3

double licz3() {
	double wynik = 0.0;
	const int N1 = abs(b - a) / e;
	const int N2 = abs(d - c) / e;
#pragma omp parallel for default(none) shared(N1, N2)
	for (int i = 1; i <= N1; i++) {
		double x = a + i * e;
		for (int j = 1; j < N2; j++) {
			double y = c + j * e;
#pragma omp atomic
			wynik += f(x, y) * e;
		}
	}

	return wynik * e;
}



};

int main() {
	omp_set_nested(1);



	CalkaPodwojna in([](double x, double y) -> double {
		return pow(M_E, -(pow(x,2)+pow(y,2)))/sqrt(1+pow(x,2)+pow(y,2));
	}, -1, 1,-1, 1, 0.001);
	




	double start = omp_get_wtime();

	std::cout << in.licz() << std::endl;

	double end = omp_get_wtime();

	double czas = end - start;


	start = omp_get_wtime();

	std::cout << in.licz1() << std::endl;

	end = omp_get_wtime();

	double czas1 = end - start;

	start = omp_get_wtime();

	std::cout << in.licz2() << std::endl;

	end = omp_get_wtime();

	double czas2 = end - start;

	start = omp_get_wtime();

	std::cout << in.licz3() << std::endl;

	end = omp_get_wtime();

	double czas3 = end - start;

	

	printf("\nBez zrownoleglenia czas wykonywania: %.16g ms\n", czas);

	printf("Zrownoleglenia 1 czas wykonywania: %.16g ms\n", czas1);

	printf("Zrownoleglenia 2 czas wykonywania: %.16g ms\n", czas2);

	printf("Zrownoleglenia 3 czas wykonywania: %.16g ms\n", czas3);


	_getch();
}