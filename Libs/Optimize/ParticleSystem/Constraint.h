#pragma once

#include "itkParticleDomain.h"	


template <unsigned int VDimension = 3>
class Constraint
{
public:
	Constraint();
	~Constraint();

	typedef itk::Point <double, VDimension> PointType;
	//typedef itk::FixedArray <T, VDimension> VectorType;

	// Apply constraint to satisfy constraint and update the point location 
	void Apply(PointType& point) = 0;
private:

};
