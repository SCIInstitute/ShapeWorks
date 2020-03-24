#pragma once

#include "itkParticleDomain.h"	
#include <iostream>

//class Constraint
//{
//public:
//	Constraint();
//	~Constraint();
//
//	typedef itk::Point <double, 3> PointType;
//	//typedef itk::FixedArray <T, VDimension> VectorType;
//
//	// Apply constraint to satisfy constraint and update the point location 
//	void Apply(PointType& point) ;
//private:
//
//};

//template <unsigned int VDimension = 3>
class Farshad
{
public:
	Farshad() { f = 1; }
	Farshad(int a) { f = a; }
	~Farshad() {}

private:
	int f;
};

class feri : public Farshad
{
public:
	feri(int a) { f = a; }
	feri() { f = 1; }
	~feri() {}

	void printFeri() { std::cout << "access to constraint class granted. f value:" << this->f << std::endl; }
	void cppPrintFeri();

private:
	int f;
};

