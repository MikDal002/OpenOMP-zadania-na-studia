// ProjektPragmaZad31.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <vector>
#include <iomanip>
#include <ostream>
#include <omp.h>
#include <ctime>

template <class T>
class Macierz {
protected:
	std::vector<std::vector<T>> v;

public:
	Macierz() : Macierz(0, 0) {}

	Macierz(size_t wiersze, size_t kolumny) {
		//Nie można zrównoleglić bo zmienna i musi posiadać typ całkowity
		for (size_t i = 0; i < wiersze; i++) {
			std::vector<T> tmp;
			for (size_t j = 0; j < kolumny; j++) {
				tmp.push_back(T(0));
			}
			v.push_back(tmp);
		}
	}

	Macierz(std::vector<std::vector<T>> v) {
		this->v = v;
	}

	size_t wiersze() const {
		return v.size();
	}

	size_t kolumny() const {
		return v.at(0).size();
	}

	std::vector<T> wiersz(unsigned int which) const {
		return v.at(which);
	}

	std::vector<T> kolumna(unsigned int which) const {
		std::vector<T> v2;

		for (size_t i = 0; i < wiersze(); i++) {
			v2.push_back(v.at(i).at(which));
		}

		return v2;
	}

	T get(size_t wiersz, size_t kolumna) const {
		return v.at(wiersz).at(kolumna);
	}

	void set(size_t wiersz, size_t kolumna, T value) {
		v.at(wiersz).at(kolumna) = value;
	}

	void mnozenie(const T& value) {
#pragma omp parallel for default(none)
		for (int i = 0; i < wiersze(); i++) {
#pragma omp parallel for default(none)
	//nie można uzyć reduction(*:v.at(i).at(j)) gdyż nie jest to zmienna 
			for (int j = 0; j < kolumny(); j++) {
				v.at(i).at(j) *= value;
			}
		}
	}

	void mnozenie(const Macierz& matrix) {
		if (kolumny() != matrix.wiersze()) {
			throw std::invalid_argument("Ilosc kolumn macierzy A nie jest równy ilosci wierszy macierzy B.");
		}

		Macierz mat(wiersze(), matrix.kolumny());
#pragma omp parallel for default(none)
		for (int i = 0; i < mat.wiersze(); i++) {
#pragma omp parallel for default(none)
			for (int j = 0; j < mat.kolumny(); j++) {
				T sum = T(0);

#pragma omp parallel for default(none) reduction(+:sum)
				for (int k = 0; k < kolumny(); k++) {
					sum += get(i, k) * matrix.get(k, j);
				}

				mat.set(i, j, sum);
			}
		}

		this->v = mat.v;
	}

	Macierz operator*(const T& value) const {
		Macierz mat(this->v);
		mat.mnozenie(value);
		return mat;
	}

	Macierz operator*(const Macierz& matrix) const {
		Macierz mat(this->v);
		mat.mnozenie(matrix);
		return mat;
	}

	friend std::ostream& operator<< (std::ostream& stream, const Macierz& matrix) {
		for (size_t i = 0; i < matrix.wiersze(); i++) {
			for (size_t j = 0; j < matrix.kolumny(); j++) {
				stream << std::left << std::setw(10) << matrix.v.at(i).at(j);
			}
			stream << std::endl;
		}
		return stream;
	}
};

#define MACIERZ_ROZMIAR 500

int main() {
	clock_t czas;

	omp_set_nested(64);
	srand(time(NULL));

	std::vector<std::vector<int>> v1, v2;
	//Nie można zrównoleglić gdyż nastąpi konflikt związany z dostepem do zasobów
	for (int i = 0; i < MACIERZ_ROZMIAR; i++) {
		std::vector<int> tmp1, tmp2;

		for (int j = 0; j < MACIERZ_ROZMIAR; j++) {
			tmp1.push_back((rand() % 19) - 9);
			tmp2.push_back((rand() % 19) - 9);
		}

		v1.push_back(tmp1);
		v2.push_back(tmp2);
	}

	Macierz<int> mat1(v1);
	Macierz<int> mat2(v2);

	//std::cout << mat1;
	//std::cout << "\n";
	//std::cout << mat2;

	czas = clock();
	mat1 * mat2;
	//std::cout << "\n";
	//std::cout << mat1*mat2;
	std::cout << "Mnozenie macierzy przez macierz: " << (double)(clock() - czas) / CLOCKS_PER_SEC << " s" << std::endl;

	czas = clock();
	mat1 * 5;
	std::cout << "Mnozenie macierzy przez wektor: " << (double)(clock() - czas) / CLOCKS_PER_SEC << " s" << std::endl;

	_getch();
}