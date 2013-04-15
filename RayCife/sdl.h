#ifndef SDL_H
#define SDL_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "quadric.h"
#include "object.h"

using namespace std;

class SDL {
	public:
		Vec eye;
		int size[2];
		double ortho[4];
		double ambiente;
		vector <Light*> lights;
		vector <Object*> objects;
		double background[3];
		double profundidade;
		string output;
		
	SDL(string nome);
};

#endif
