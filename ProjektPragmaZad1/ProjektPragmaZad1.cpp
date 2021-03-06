// ProjektPragmaZad1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <ctime>
#define _USE_MATH_DEFINES // for C++  
#include <math.h>
#include <omp.h>

using namespace std;


double licz(double x, int k) {
	return pow(M_E, (sqrt(k*x)))/(1+pow(x,2));
}

int main() {
	omp_set_nested(1);
	float xp, xk;
	int dokladnosc;
	double wysokosc;
	double k1[100], k2[100], k3[100],k4[100];

	// przedzialy
	xp = 0;
	xk = 2*M_PI;

	dokladnosc = 50000;
	wysokosc = (xk - xp) / dokladnosc;


//bez zrównoleglenia

	double start = omp_get_wtime();

	for (int k = 1; k <= 100; k++) {
		double suma = 0;

		for (int x = 0; x <= dokladnosc; x++) {

			suma += licz(x * wysokosc, k) * wysokosc;
		}
		k1[k] = suma;
		
	}
	double end = omp_get_wtime();
	double czas1 = end - start;




	//zrównoleglenie 1
	 start = omp_get_wtime();
#pragma omp parallel for  default(none)  
	for (int k = 1; k <= 100; k++) {
		double suma = 0;
#pragma omp parallel for default(none) 
		for (int x = 0; x <= dokladnosc; x++) {
#pragma omp atomic
			suma += licz(x * wysokosc, k) * wysokosc;
		}
#pragma omp critical
		k2[k] = suma;
	}
	 end = omp_get_wtime();
	 double czas2 = end - start;
	



	//zrównoleglenie 2
	 omp_lock_t my_lock;
	 omp_init_lock(&my_lock);
	 start = omp_get_wtime();
#pragma omp parallel for  default(none) 
	 for (int k = 1; k <= 100; k++) {
		 double suma = 0;
#pragma omp parallel for default(none) reduction(+ : suma) 
		 for (int x = 0; x <= dokladnosc; x++) {

			 suma += licz(x * wysokosc, k) * wysokosc;
		 }
		 omp_set_lock(&my_lock);
		 k3[k] = suma;
		 omp_unset_lock(&my_lock);
	 }
	 omp_destroy_lock(&my_lock);

	 end = omp_get_wtime();
	 double czas3 = end - start;


	 //zrównoleglenie 3 (naj)

	 omp_init_lock(&my_lock);
	 start = omp_get_wtime();
#pragma omp parallel for  default(none) 
	 for (int k = 1; k <= 100; k++) {
		 double suma = 0;

		 for (int x = 0; x <= dokladnosc; x++) {

			 suma += licz(x * wysokosc, k) * wysokosc;
		 }
		 omp_set_lock(&my_lock);
		 k4[k] = suma;
		 omp_unset_lock(&my_lock);
	 }
	 omp_destroy_lock(&my_lock);

	 end = omp_get_wtime();
	 double czas4 = end - start;

	 cout << "	      " << "Szeregowo"<< "     " <<"Zrow 1" << "     " << "Zrow 2" << "     " <<" Zrow 3" << endl;
	 for (int k = 1; k <= 100; k++) {
		 cout << "k = " << k << ", c(k) = " << k1[k] << "     " << k2[k] << "     " << k3[k] << "     " << k4[k] << endl;
	 }

	 printf("\nBez zrownoleglenia czas wykonywania: %.16g s\n", czas1);
	 printf("Zrownoleglenie 1 czas  wykonywania : %.16g s\n", czas2);
	 printf("Zrownoleglenie 2 czas  wykonywania : %.16g s\n", czas3);
	 printf("Zrownoleglenie 3 czas  wykonywania : %.16g s\n", czas4);
	_getch();
}