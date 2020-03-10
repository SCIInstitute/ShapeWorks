#pragma once

#include "Constraint.h"	


template <unsigned int VDimension = 3>
class PlanarConstraint : public Constraint
{
public:
	typedef Constraint<VDimension>::PointType PointType;

	PlanarConstraint(PointType point1, PointType point2, PointType point3) {
		
	}
	~PlanarConstraint();

	// gneerate the plane normal
	//typedef itk::FixedArray<T, VDimension> VectorType;


	// Apply constraint to satisfy constraint and update the point location 
	void Apply(PointType& point){
		// project the point to the constraint plane
	}
private:
	PointType point1, point2, point3;
	VectorType planeNormal



};
