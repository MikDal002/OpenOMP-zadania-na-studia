// ProjektPragmaZad4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <conio.h>
using namespace std;

int main()
{
	ifstream plik("C:\\Users\\Dalec\\Desktop\\Yes.wav", ios::binary);

	char Nagl[44];
	short wart;

	plik.read(Nagl, 44);
	while (1)
	{
		plik.read(reinterpret_cast< char * >(&wart), 2);
		if (!plik) break;
		cout << wart << "\n";


	}
	plik.close();

	_getch();
}

