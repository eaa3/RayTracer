#ifndef OBJECT_H
#define OBJECT_H

#include "quadric.h"

class Object{
	public:
		Quad *quad;
		double KS;
		double KT;
		double ir;

	Object();
};

#endif